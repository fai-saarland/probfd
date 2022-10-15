#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/bisimulation/engine_interfaces.h"

#include "probfd/probabilistic_operator.h"
#include "probfd/globals.h"

#include "algorithms/segmented_vector.h"

#include "merge_and_shrink/abstraction.h"
#include "merge_and_shrink/linear_merge_strategy.h"
#include "merge_and_shrink/merge_and_shrink_heuristic.h"
#include "merge_and_shrink/multiple_shrinking_strategies.h"
#include "merge_and_shrink/shrink_bisimulation.h"
#include "merge_and_shrink/shrink_budget_based.h"
#include "merge_and_shrink/shrink_strategy.h"
#include "merge_and_shrink/variable_order_finder.h"

#include "utils/hash.h"
#include "utils/system.h"
#include "utils/timer.h"

#include "global_operator.h"
#include "global_state.h"
#include "option_parser.h"

#include <cassert>
#include <map>
#include <memory>
#include <unordered_set>
#include <vector>

using namespace merge_and_shrink;

namespace probfd {
namespace bisimulation {

static constexpr const int BUCKET_SIZE = 1024 * 64;

static bool is_dummy_outcome(const GlobalOperator* op)
{
    return op->get_id() >= ::g_operators.size();
}

static unsigned get_global_operator_id(const GlobalOperator* op)
{
    return op->get_id();
}

std::size_t BisimilarStateSpace::Hash::operator()(unsigned i) const
{
    return utils::get_hash<elem_type>(extended_->operator[](i));
}

bool BisimilarStateSpace::Equal::operator()(int i, int j) const
{
    return extended_->operator[](i) == extended_->operator[](j);
}

BisimilarStateSpace::BisimilarStateSpace(
    const GlobalState& initial_state,
    int budget,
    OperatorCost cost_type)
    : limited_budget_(budget != g_unlimited_budget)
    , cost_type_(cost_type)
    , abstraction_(nullptr)
    , initial_state_(::StateID::no_state)
    , dead_end_state_(::StateID::no_state)
    , transitions_()
    , extended_()
    , ids_(1024, Hash(&extended_), Equal(&extended_))
{
    utils::Timer timer_total;
    utils::Timer timer;

    std::cout << "Computing all-outcomes determinization bisimulation..."
              << std::endl;

    options::Options opts_bisim;
    opts_bisim.set<int>("max_states", std::numeric_limits<int>::max());
    opts_bisim.set<int>(
        "max_states_before_merge",
        std::numeric_limits<int>::max());
    opts_bisim.set<int>("greedy", 0);
    opts_bisim.set<int>("threshold", 1);
    opts_bisim.set<bool>("initialize_by_h", true);
    opts_bisim.set<bool>("group_by_h", false);
    opts_bisim.set<int>("at_limit", 0);
    std::shared_ptr<ShrinkStrategy> bisim =
        std::make_shared<ShrinkBisimulation>(opts_bisim);

    if (limited_budget_) {
        options::Options opts;
        opts.set<int>("max_states", std::numeric_limits<int>::max());
        opts.set<int>(
            "max_states_before_merge",
            std::numeric_limits<int>::max());
        opts.set<int>("budget", budget);
        std::shared_ptr<ShrinkStrategy> bs =
            std::make_shared<BudgetShrink>(opts);
        std::vector<std::shared_ptr<ShrinkStrategy>> s;
        s.push_back(bisim);
        s.push_back(bs);
        opts.set<std::vector<std::shared_ptr<ShrinkStrategy>>>("strategies", s);
        bisim = std::make_shared<MultipleShrinkingStrategy>(opts);
    }

    options::Options opts;
    opts.set<int>("cost_type", cost_type_);
    opts.set<bool>("release_memory", false);
    opts.set<int>("count", 1);
    opts.set<int>("merge_strategy", MERGE_LINEAR);
    opts.set<int>("merge_order", LEVEL);
    opts.set<std::shared_ptr<ShrinkStrategy>>("shrink_strategy", bisim);
    opts.set<std::vector<std::shared_ptr<MergeCriterion>>>(
        "merge_criteria",
        std::vector<std::shared_ptr<MergeCriterion>>());
    opts.set<bool>("reduce_labels", false);
    opts.set<bool>("prune_unreachable", true);
    opts.set<bool>("prune_irrelevant", true);
    opts.set<bool>("expensive_statistics", false);
    opts.set<bool>("label_inheritance", false);
    opts.set<bool>("new_approach", false);
    opts.set<bool>("use_empty_label_shrinking", false);
    opts.set<bool>("use_uniform_distances", false);
    opts.set<bool>("check_unsolvability", false);
    opts.set<bool>("continue_if_unsolvable", false);
    opts.set<int>("merge_size_limit", std::numeric_limits<int>::max());
    opts.set<int>("max_branching_merge", std::numeric_limits<int>::max());

    num_cached_transitions_ = 0;
    std::unique_ptr<MergeAndShrinkHeuristic> ms =
        std::unique_ptr<MergeAndShrinkHeuristic>(
            new MergeAndShrinkHeuristic(opts));
    ms->evaluate(initial_state);

    std::cout << "AOD-bisimulation was constructed in " << timer << std::endl;
    timer.reset();

    if (!ms->is_dead_end()) {
        assert(ms->get_abstractions().size() == 1);
        abstraction_ = ms->get_abstractions()[0];

        transitions_.resize(
            abstraction_->size(),
            std::vector<CachedTransition>());

        std::vector<std::vector<std::pair<unsigned, unsigned>>> g_to_p(
            ::g_operators.size());
        std::vector<std::vector<unsigned>> dummys(g_operators.size());
        for (unsigned p_op_id = 0; p_op_id < g_operators.size(); ++p_op_id) {
            const ProbabilisticOperator& op = g_operators[p_op_id];
            for (unsigned i = 0; i < op.num_outcomes(); ++i) {
                if (!is_dummy_outcome(op[i].op)) {
                    g_to_p[get_global_operator_id(op[i].op)].emplace_back(
                        p_op_id,
                        i);
                } else {
                    dummys[p_op_id].push_back(i);
                }
            }
        }

        unsigned bucket_free = 0;
        int* bucket_ptr = nullptr;
        auto allocate = [this, &bucket_free, &bucket_ptr](unsigned size) {
            if (size > bucket_free) {
                store_.push_back(new int[BUCKET_SIZE]);
                bucket_ptr = store_.back();
                bucket_free = BUCKET_SIZE;
            }
            int* result = bucket_ptr;
            bucket_ptr += size;
            bucket_free -= size;
            return result;
        };

#if 1
        std::vector<std::vector<merge_and_shrink::AbstractTransition>>
            transitions(abstraction_->extract_transitions());

        for (int g_op_id = g_to_p.size() - 1; g_op_id >= 0; --g_op_id) {
            std::vector<merge_and_shrink::AbstractTransition> op_transitions(
                std::move(transitions.back()));
            transitions.pop_back();
            for (int i = op_transitions.size() - 1; i >= 0; --i) {
                const AbstractTransition& trans = op_transitions[i];
                std::vector<CachedTransition>& ts = transitions_[trans.src];
                if (trans.target != Abstraction::PRUNED_STATE &&
                    trans.target != trans.src) {
                    for (const auto& op : g_to_p[g_op_id]) {
                        CachedTransition* t = nullptr;
                        for (int j = ts.size() - 1; j >= 0; --j) {
                            if (ts[j].op == op.first) {
                                t = &ts[j];
                                break;
                            }
                        }
                        if (t == nullptr) {
                            const unsigned size =
                                g_operators[op.first].num_outcomes();
                            ts.emplace_back();
                            t = &ts.back();
                            t->op = op.first;
                            t->successors = allocate(size);
                            for (int j = size - 1; j >= 0; --j) {
                                t->successors[j] = Abstraction::PRUNED_STATE;
                            }
                            for (int j = dummys[op.first].size() - 1; j >= 0;
                                 --j) {
                                t->successors[dummys[op.first][j]] = trans.src;
                            }
                            ++num_cached_transitions_;
                        }
                        t->successors[op.second] = trans.target;
                    }
                }
            }
        }

#else
        for (unsigned p_op_id = 0; p_op_id < g_operators.size(); ++p_op_id) {
            const ProbabilisticOperator& op = g_operators[p_op_id];
            for (unsigned i = 0; i < op.num_outcomes(); ++i) {
                if (is_dummy_outcome(op[i].op)) {
                    continue;
                }
                unsigned g_op_id = get_global_operator_id(op[i].op);
                for (const auto& trans :
                     abstraction_->get_transitions_for_op(g_op_id)) {
                    if (trans.target == Abstraction::PRUNED_STATE ||
                        trans.target == trans.src) {
                        continue;
                    }
                    assert((unsigned)trans.src < transitions_.size());
                    std::vector<CachedTransition>& ts = transitions_[trans.src];
                    if (ts.empty() || ts.back().op != p_op_id) {
                        ts.emplace_back();
                        CachedTransition& t = ts.back();
                        t.op = p_op_id;
                        // t.successors = new int[op.num_outcomes()];
                        // for (int j = op.num_outcomes() - 1; j >= 0; --j) {
                        //     t.successors[j] = Abstraction::PRUNED_STATE;
                        // }
                        const unsigned size = op.num_outcomes();
                        t.successors = allocate(size);
                        for (int j = size - 1; j >= 0; --j) {
                            t.successors[j] = Abstraction::PRUNED_STATE;
                        }
                        for (int j = dummys[p_op_id].size() - 1; j >= 0; --j) {
                            t.successors[dummys[p_op_id][j]] = trans.src;
                        }
                        ++num_cached_transitions_;
                    }
                    ts.back().successors[i] = trans.target;
                }
            }

#if 0
            for (unsigned i = 0; i < op.num_outcomes(); ++i) {
                unsigned g_op_id = get_global_operator_id(op[i].op);
                if (g_op_id > ::g_operators.size()) {
                    for (unsigned src = 0; src < transitions_.size(); ++src) {
                        std::vector<CachedTransition>& ts = transitions_[src];
                        if (!ts.empty() && ts.back().op == p_op_id) {
                            ts[i] = src;
                        }
                    }
                }
            }
#endif
        }
#endif

        abstraction_->release_memory();

#if 0
        for (unsigned s = 0; s < transitions_.size(); ++s) {
            std::vector<CachedTransition>& ts = transitions_[s];
            unsigned j = 0;
            for (unsigned i = 0; i < ts.size(); ++i) {
                bool self = true;
                CachedTransition& t = ts[i];
                const ProbabilisticOperator& op = g_operators[t.op];
                for (int k = op.num_outcomes() - 1; k >= 0; --k) {
                    if (is_dummy_outcome(op[k].op)) {
                        t.successors[k] = s;
                    } else if (
                        t.successors[k] != Abstraction::PRUNED_STATE
                        && t.successors[k] != (int)s) {
                        self = false;
                    }
                }
                if (!self) {
                    ts[j] = ts[i];
                    ++j;
                }
            }
            ts.resize(j);
            ts.shrink_to_fit();
        }
#endif

        if (limited_budget_) {
            extended_.push_back(std::pair<int, int>(
                budget,
                abstraction_->get_abstract_state(initial_state)));
            ids_.insert(0);
            initial_state_ = QuotientState(0);
            extended_.push_back(
                std::pair<int, int>(budget, Abstraction::PRUNED_STATE));
            ids_.insert(1);
            dead_end_state_ = QuotientState(1);

            operator_cost_.resize(g_operators.size(), 0);
            for (int i = g_operators.size() - 1; i >= 0; --i) {
                operator_cost_[i] = ::get_adjusted_action_cost(
                    *g_operators[i].get(0).op,
                    cost_type_);
            }
        } else {
            initial_state_ =
                QuotientState(abstraction_->get_abstract_state(initial_state));
            dead_end_state_ = QuotientState(transitions_.size());
        }
    } else {
        initial_state_ = QuotientState(0);
        dead_end_state_ = QuotientState(0);
    }

    std::cout << "Rebuilt probabilistic bisimulation in " << timer << std::endl;
    std::cout << "Total time for computing probabilistic bisimulation: "
              << timer_total << std::endl;
}

BisimilarStateSpace::~BisimilarStateSpace()
{
    if (abstraction_ != nullptr) {
        delete (abstraction_);
    }
    for (int i = store_.size() - 1; i >= 0; --i) {
        delete[] (store_[i]);
    }
}

QuotientState BisimilarStateSpace::get_initial_state() const
{
    return initial_state_;
}

bool BisimilarStateSpace::has_limited_budget() const
{
    return limited_budget_;
}

bool BisimilarStateSpace::is_goal_state(const QuotientState& s) const
{
    if (s == dead_end_state_) {
        return false;
    } else if (limited_budget_) {
        const std::pair<int, int>& orig = extended_[s.hash()];
        return abstraction_->is_goal_state(orig.second);
    } else {
        return abstraction_->is_goal_state(s.hash());
    }
}

bool BisimilarStateSpace::is_dead_end(const QuotientState& s) const
{
    return s == dead_end_state_;
}

void BisimilarStateSpace::get_applicable_actions(
    const StateID& s,
    std::vector<QuotientAction>& result) const
{
    if (s == dead_end_state_.hash()) {
        return;
    }
    if (limited_budget_) {
        assert(s < extended_.size());
        const std::pair<int, int>& orig = extended_[s];
        assert(orig.second != Abstraction::PRUNED_STATE);
        const std::vector<CachedTransition>& cache = transitions_[orig.second];
        result.reserve(cache.size());
        for (unsigned i = 0; i < cache.size(); ++i) {
            if (operator_cost_[cache[i].op] <= orig.first) {
                result.emplace_back(i);
            }
        }
    } else {
        const std::vector<CachedTransition>& cache = transitions_[s];
        result.reserve(cache.size());
        for (unsigned i = 0; i < cache.size(); ++i) {
            result.emplace_back(i);
        }
    }
}

void BisimilarStateSpace::get_successors(
    const StateID& s,
    const QuotientAction& a,
    Distribution<StateID>& result)
{
    assert(s != dead_end_state_.hash());
    if (limited_budget_) {
        const std::pair<int, int>& orig = extended_[s];
        assert(orig.second != Abstraction::PRUNED_STATE);
        assert(a.idx < transitions_[orig.second].size());
        const CachedTransition& t = transitions_[orig.second][a.idx];
        const ProbabilisticOperator& op = g_operators[t.op];
        assert(t.op == op.get_id());
        const int new_b = orig.first - operator_cost_[t.op];
        assert(new_b >= 0);
        for (unsigned i = 0; i < op.num_outcomes(); ++i) {
            if (t.successors[i] == Abstraction::PRUNED_STATE ||
                new_b < abstraction_->get_goal_distance(t.successors[i])) {
                result.add(StateID(dead_end_state_.hash()), op[i].prob);
            } else {
                result.add(
                    StateID(get_budget_extended_state(t.successors[i], new_b)
                                .hash()),
                    op[i].prob);
            }
        }
    } else {
        assert(a.idx < transitions_[s].size());
        const CachedTransition& t = transitions_[s][a.idx];
        const ProbabilisticOperator& op = g_operators[t.op];
        for (unsigned i = 0; i < op.num_outcomes(); ++i) {
            if (t.successors[i] == Abstraction::PRUNED_STATE) {
                result.add(StateID(dead_end_state_.hash()), op[i].prob);
            } else {
                result.add(StateID(t.successors[i]), op[i].prob);
            }
        }
    }
}

QuotientState BisimilarStateSpace::get_budget_extended_state(
    const int& ref,
    const int& budget)
{
    assert(limited_budget_);
    assert(ref != Abstraction::PRUNED_STATE);
    assert(extended_.size() == ids_.size());
    extended_.push_back(std::pair<int, int>(budget, ref));
    auto inserted = ids_.insert(extended_.size() - 1);
    if (!inserted.second) {
        extended_.pop_back();
    }
    return QuotientState(*inserted.first);
}

unsigned BisimilarStateSpace::num_bisimilar_states() const
{
    return abstraction_ != nullptr ? abstraction_->size() : 1;
}

unsigned BisimilarStateSpace::num_extended_states() const
{
    return limited_budget_ ? extended_.size() : num_bisimilar_states();
}

unsigned BisimilarStateSpace::num_transitions() const
{
    return num_cached_transitions_;
}

void BisimilarStateSpace::dump(std::ostream& out) const
{
    out << "digraph {"
        << "\n";

    if (initial_state_ == dead_end_state_) {
        out << "init [shape=ellipse, label=\"dead\"];"
            << "\n";
        out << "-> init;"
            << "\n";
        out << "}" << std::flush;
        return;
    }

    for (unsigned node = 0; node < transitions_.size(); ++node) {
        out << "n" << node << " [shape=circle, label=\"#" << node << "\""
            << (abstraction_->is_goal_state(node) ? ", peripheries=2" : "")
            << "];\n";
    }
    out << "n" << transitions_.size() << " [shape=circle, label=\"dead\"];\n";

    unsigned t = 0;
    for (unsigned node = 0; node < transitions_.size(); ++node) {
        const std::vector<CachedTransition>& ts = transitions_[node];
        for (unsigned i = 0; i < ts.size(); ++i) {
            out << "t" << t << " [shape=rectangle, label=\""
                << g_operators[ts[i].op].get_name() << "\"];\n";
            ++t;
        }
    }

    out << "\n";
    out << "\"\" -> n"
        << (limited_budget_ ? extended_[initial_state_.hash()].second
                            : initial_state_.hash())
        << "\n";

    t = 0;
    for (unsigned node = 0; node < transitions_.size(); ++node) {
        const std::vector<CachedTransition>& ts = transitions_[node];
        for (unsigned i = 0; i < ts.size(); ++i) {
            out << "n" << node << " -> t" << t << ";\n";
            Distribution<int> succs;
            for (unsigned j = 0; j < g_operators[ts[i].op].num_outcomes();
                 ++j) {
                const int succ = ts[i].successors[j];
                succs.add(
                    (succ == Abstraction::PRUNED_STATE
                         ? (int)transitions_.size()
                         : succ),
                    g_operators[ts[i].op][j].prob);
            }
            for (auto it = succs.begin(); it != succs.end(); ++it) {
                out << "t" << t << " -> n" << it->first << "\n";
            }
            ++t;
        }
    }

    out << "}" << std::flush;
}

} // namespace bisimulation
} // namespace probfd
