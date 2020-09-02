#include "bisimilar_state_space.h"

#include "../../global_operator.h"
#include "../../global_state.h"
#include "../../globals.h"
#include "../../merge_and_shrink/abstraction.h"
#include "../../merge_and_shrink/linear_merge_strategy.h"
#include "../../merge_and_shrink/merge_and_shrink_heuristic.h"
#include "../../merge_and_shrink/multiple_shrinking_strategies.h"
#include "../../merge_and_shrink/shrink_bisimulation.h"
#include "../../merge_and_shrink/shrink_budget_based.h"
#include "../../merge_and_shrink/shrink_strategy.h"
#include "../../merge_and_shrink/variable_order_finder.h"
#include "../../option_parser.h"
#include "../../utils/hash.h"
#include "../../utils/timer.h"
#include "../globals.h"
#include "../probabilistic_operator.h"

#include <cassert>
#include <map>
#include <memory>
#include <vector>

using namespace merge_and_shrink;

namespace probabilistic {
namespace bisimulation {

static bool
is_dummy_outcome(const GlobalOperator* op)
{
    return op->get_id() >= ::g_operators.size();
}

static unsigned
get_global_operator_id(const GlobalOperator* op)
{
    return op->get_id();
}

std::size_t
BisimilarStateSpace::Hash::operator()(unsigned i) const
{
    return utils::get_hash<elem_type>(extended_->operator[](i));
}

BisimilarStateSpace::BisimilarStateSpace(
    const GlobalState& initial_state,
    int budget,
    OperatorCost cost_type)
    : limited_budget_(budget != g_unlimited_budget)
    , cost_type_(cost_type)
    , abstraction_(nullptr)
    , initial_state_(StateID::no_state)
    , dead_end_state_(StateID::no_state)
    , transitions_()
    , extended_()
    , ids_(1024, Hash(&extended_))
{
    utils::Timer timer_total;
    utils::Timer timer;

    std::cout << "Computing all-outcomes determinization bisimulation..." << std::endl;

    options::Options opts_bisim;
    opts_bisim.set<int>("max_states", std::numeric_limits<int>::max());
    opts_bisim.set<int>(
        "max_states_before_merge", std::numeric_limits<int>::max());
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
            "max_states_before_merge", std::numeric_limits<int>::max());
        opts.set<int>("budget", budget);
        std::shared_ptr<ShrinkStrategy> bs =
            std::make_shared<BudgetShrink>(opts);
        std::vector<std::shared_ptr<ShrinkStrategy>> s;
        s.push_back(bs);
        s.push_back(bisim);
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
        "merge_criteria", std::vector<std::shared_ptr<MergeCriterion>>());
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
            abstraction_->size(), std::vector<CachedTransition>());
        for (unsigned p_op_id = 0; p_op_id < g_operators.size(); ++p_op_id) {
            const ProbabilisticOperator& op = g_operators[p_op_id];
            for (unsigned i = 0; i < op.num_outcomes(); ++i) {
                if (is_dummy_outcome(op[i].op)) {
                    continue;
                }
                unsigned g_op_id = get_global_operator_id(op[i].op);
                for (const auto& trans :
                     abstraction_->get_transitions_for_op(g_op_id)) {
                    assert((unsigned)trans.src < transitions_.size());
                    std::vector<CachedTransition>& ts = transitions_[trans.src];
                    if (ts.empty() || ts.back().op != p_op_id) {
                        ts.emplace_back();
                        CachedTransition& t = ts.back();
                        t.op = p_op_id;
                        t.successors = new int[op.num_outcomes()];
                        for (int j = op.num_outcomes() - 1; j >= 0; --j) {
                            t.successors[j] = Abstraction::PRUNED_STATE;
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
        abstraction_->release_memory();

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

        if (limited_budget_) {
            extended_.push_back(std::pair<int, int>(
                budget, abstraction_->get_abstract_state(initial_state)));
            ids_.insert(0);
            initial_state_ = QuotientState(0);
            extended_.push_back(
                std::pair<int, int>(budget, Abstraction::PRUNED_STATE));
            ids_.insert(1);
            dead_end_state_ = QuotientState(1);
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
    for (int i = transitions_.size() - 1; i >= 0; --i) {
        for (int j = transitions_[i].size() - 1; j >= 0; --j) {
            delete[](transitions_[i][j].successors);
        }
    }
}

QuotientState
BisimilarStateSpace::get_initial_state() const
{
    return initial_state_;
}

bool
BisimilarStateSpace::has_limited_budget() const
{
    return limited_budget_;
}

bool
BisimilarStateSpace::is_goal_state(const QuotientState& s) const
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

bool
BisimilarStateSpace::is_dead_end(const QuotientState& s) const
{
    return s == dead_end_state_;
}

void
BisimilarStateSpace::get_applicable_actions(
    const QuotientState& s,
    std::vector<QuotientAction>& result) const
{
    if (s == dead_end_state_) {
        return;
    }
    if (limited_budget_) {
        assert(s.hash() < extended_.size());
        const std::pair<int, int>& orig = extended_[s.hash()];
        assert(orig.second != Abstraction::PRUNED_STATE);
        const std::vector<CachedTransition>& cache = transitions_[orig.second];
        result.reserve(cache.size());
        for (unsigned i = 0; i < cache.size(); ++i) {
            result.emplace_back(i);
        }
    } else {
        const std::vector<CachedTransition>& cache = transitions_[s.hash()];
        result.reserve(cache.size());
        for (unsigned i = 0; i < cache.size(); ++i) {
            result.emplace_back(i);
        }
    }
}

Distribution<QuotientState>
BisimilarStateSpace::get_successors(
    const QuotientState& s,
    const QuotientAction& a)
{
    assert(s != dead_end_state_);
    if (limited_budget_) {
        const std::pair<int, int>& orig = extended_[s.hash()];
        assert(orig.second != Abstraction::PRUNED_STATE);
        assert(a.idx < transitions_[orig.second].size());
        const CachedTransition& t = transitions_[orig.second][a.idx];
        const ProbabilisticOperator& op = g_operators[t.op];
        Distribution<QuotientState> result;
        assert(t.op == op.get_id());
        for (unsigned i = 0; i < op.num_outcomes(); ++i) {
            int new_b = orig.first
                - ::get_adjusted_action_cost(*(op[i].op), cost_type_);
            if (new_b < 0 || t.successors[i] == Abstraction::PRUNED_STATE
                || new_b < abstraction_->get_goal_distance(t.successors[i])) {
                result.add(dead_end_state_, op[i].prob);
            } else {
                result.add(
                    get_budget_extended_state(t.successors[i], new_b),
                    op[i].prob);
            }
        }
        return result;
    } else {
        assert(a.idx < transitions_[s.hash()].size());
        const CachedTransition& t = transitions_[s.hash()][a.idx];
        const ProbabilisticOperator& op = g_operators[t.op];
        Distribution<QuotientState> result;
        for (unsigned i = 0; i < op.num_outcomes(); ++i) {
            if (t.successors[i] == Abstraction::PRUNED_STATE) {
                result.add(dead_end_state_, op[i].prob);
            } else {
                result.add(QuotientState(t.successors[i]), op[i].prob);
            }
        }
        return result;
    }
}

QuotientState
BisimilarStateSpace::get_budget_extended_state(
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

unsigned
BisimilarStateSpace::num_bisimilar_states() const
{
    return abstraction_ != nullptr ? abstraction_->size() : 1;
}

unsigned
BisimilarStateSpace::num_extended_states() const
{
    return limited_budget_ ? extended_.size() : num_bisimilar_states();
}

unsigned
BisimilarStateSpace::num_transitions() const
{
    return num_cached_transitions_;
}

void
BisimilarStateSpace::dump(std::ostream& out) const
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

namespace algorithms {

StateID
StateIDMap<bisimulation::QuotientState>::operator[](
    const bisimulation::QuotientState& s) const
{
    return s;
}

bisimulation::QuotientState
StateIDMap<bisimulation::QuotientState>::operator[](const StateID& s) const
{
    return bisimulation::QuotientState(s);
}

ApplicableActionsGenerator<
    bisimulation::QuotientState,
    bisimulation::QuotientAction>::
    ApplicableActionsGenerator(bisimulation::BisimilarStateSpace* bisim)
    : bisim_(bisim)
{
}

void
ApplicableActionsGenerator<
    bisimulation::QuotientState,
    bisimulation::QuotientAction>::
operator()(
    const bisimulation::QuotientState& s,
    std::vector<bisimulation::QuotientAction>& res) const
{
    bisim_->get_applicable_actions(s, res);
}

TransitionGenerator<bisimulation::QuotientState, bisimulation::QuotientAction>::
    TransitionGenerator(bisimulation::BisimilarStateSpace* bisim)
    : bisim_(bisim)
{
}

Distribution<bisimulation::QuotientState>
TransitionGenerator<bisimulation::QuotientState, bisimulation::QuotientAction>::
operator()(
    const bisimulation::QuotientState& s,
    const bisimulation::QuotientAction& a) const
{
    return bisim_->get_successors(s, a);
}

StateEvaluationFunction<bisimulation::QuotientState>::StateEvaluationFunction(
    bisimulation::BisimilarStateSpace* bisim,
    const value_type::value_t min,
    const value_type::value_t max)
    : bisim_(bisim)
    , min_(min)
    , max_(max)
{
}

EvaluationResult
StateEvaluationFunction<bisimulation::QuotientState>::operator()(
    const bisimulation::QuotientState& s) const
{
    if (bisim_->is_dead_end(s)) {
        return EvaluationResult(true, min_);
    }
    if (bisim_->is_goal_state(s)) {
        return EvaluationResult(true, max_);
    }
    return EvaluationResult(false, value_type::zero);
}

value_type::value_t
TransitionRewardFunction<
    bisimulation::QuotientState,
    bisimulation::QuotientAction>::
operator()(
    const bisimulation::QuotientState&,
    const bisimulation::QuotientAction&) const
{
    return value_type::zero;
}
} // namespace algorithms

} // namespace probabilistic
