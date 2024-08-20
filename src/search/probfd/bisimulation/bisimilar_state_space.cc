#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/distribution.h"
#include "probfd/transition.h"

#include "downward/merge_and_shrink/distances.h"
#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_and_shrink_algorithm.h"
#include "downward/merge_and_shrink/merge_and_shrink_representation.h"
#include "downward/merge_and_shrink/merge_strategy_factory_precomputed.h"
#include "downward/merge_and_shrink/merge_tree.h"
#include "downward/merge_and_shrink/merge_tree_factory_linear.h"
#include "downward/merge_and_shrink/shrink_bisimulation.h"
#include "downward/merge_and_shrink/transition_system.h"
#include "downward/merge_and_shrink/types.h"

#include "downward/task_utils/variable_order_finder.h"

#include "downward/utils/logging.h"
#include "downward/utils/timer.h"

#include "downward/plugins/options.h"

#include "downward/task_proxy.h"

#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>

class AbstractTask;
namespace merge_and_shrink {
class MergeStrategyFactory;
class MergeTreeFactory;
class ShrinkStrategy;
} // namespace merge_and_shrink

using namespace merge_and_shrink;

namespace probfd {
class ProbabilisticTask;

namespace bisimulation {

static constexpr const int BUCKET_SIZE = 1024 * 64;

BisimilarStateSpace::BisimilarStateSpace(
    const ProbabilisticTask* task,
    value_t upper_bound)
    : task_proxy_(*task)
    , upper_bound_(upper_bound)
{
    utils::Timer timer_total;
    utils::Timer timer;

    std::cout << "Computing all-outcomes determinization bisimulation..."
              << std::endl;

    // Construct a linear merge tree
    auto linear_merge_tree_factory = std::make_shared<MergeTreeFactoryLinear>(
        variable_order_finder::VariableOrderType::LEVEL,
        -1,
        UpdateOption::USE_FIRST);

    // Construct the merge strategy factory
    auto merge_strategy_factory =
        std::make_shared<MergeStrategyFactoryPrecomputed>(
            linear_merge_tree_factory,
            utils::Verbosity::SILENT);

    // Construct a bisimulation-based shrinking strategy
    auto shrinking =
        std::make_shared<ShrinkBisimulation>(false, AtLimit::RETURN);

    MergeAndShrinkAlgorithm mns_algorithm(
        merge_strategy_factory,
        shrinking,
        nullptr,
        true,
        true,
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<double>::infinity(),
        utils::Verbosity::SILENT);

    num_cached_transitions_ = 0;

    const AbstractTask& determinization =
        task_properties::get_determinization(task);

    TaskProxy det_task_proxy(determinization);

    FactoredTransitionSystem fts =
        mns_algorithm.build_factored_transition_system(det_task_proxy);

    std::cout << "AOD-bisimulation was constructed in " << timer << std::endl;
    timer.reset();

    assert(fts.get_num_active_entries() == 1);

    const int last_index = static_cast<int>(fts.get_size() - 1);

    if (fts.is_factor_solvable(last_index)) {
        const merge_and_shrink::TransitionSystem& abstraction =
            fts.get_transition_system(last_index);

        goal_flags_.resize(abstraction.get_size(), false);

        for (std::size_t i = 0; i != goal_flags_.size(); ++i) {
            goal_flags_[i] = abstraction.is_goal_state(i);
        }

        transitions_.resize(
            abstraction.get_size(),
            std::vector<CachedTransition>());

        OperatorsProxy det_operators = det_task_proxy.get_operators();
        ProbabilisticOperatorsProxy prob_operators =
            task_proxy_.get_operators();

        std::vector<std::pair<unsigned, unsigned>> g_to_p(det_operators.size());
        for (unsigned p_op_id = 0; p_op_id < prob_operators.size(); ++p_op_id) {
            const ProbabilisticOperatorProxy op = prob_operators[p_op_id];
            const ProbabilisticOutcomesProxy outcomes = op.get_outcomes();
            for (unsigned i = 0; i < outcomes.size(); ++i) {
                const ProbabilisticOutcomeProxy outcome = outcomes[i];
                // if (!is_dummy_outcome(op[i].op)) {
                g_to_p[outcome.get_determinization_id()] =
                    std::make_pair(p_op_id, i);
                //} else {
                //    dummys[p_op_id] = i;
                //}
            }
        }

        unsigned bucket_free = 0;
        int* bucket_ptr = nullptr;
        auto allocate = [this, &bucket_free, &bucket_ptr](unsigned size) {
            if (size > bucket_free) {
                bucket_ptr = store_.emplace_back(new int[BUCKET_SIZE]).get();
                bucket_free = BUCKET_SIZE;
            }
            int* result = bucket_ptr;
            bucket_ptr += size;
            bucket_free -= size;
            return result;
        };

        for (const LocalLabelInfo& local_info : abstraction) {
            for (const int g_op_id : local_info.get_label_group()) {
                for (const auto& trans : local_info.get_transitions()) {
                    std::vector<CachedTransition>& ts = transitions_[trans.src];
                    assert(trans.target != PRUNED_STATE);
                    // if (trans.target == PRUNED_STATE ||
                    //     trans.target == trans.src) continue;
                    const auto& op = g_to_p[g_op_id];

                    CachedTransition* t = nullptr;
                    for (auto& j : ts) {
                        if (j.op == op.first) {
                            t = &j;
                            break;
                        }
                    }

                    if (t == nullptr) {
                        const OperatorID id = OperatorID(op.first);
                        const int size =
                            prob_operators[id].get_outcomes().size();
                        t = &ts.emplace_back();
                        t->op = op.first;
                        t->successors = allocate(size);
                        for (int j = 0; j != size; ++j) {
                            t->successors[j] = PRUNED_STATE;
                        }
                        ++num_cached_transitions_;
                    }

                    t->successors[op.second] = trans.target;
                }
            }
        }

        auto factor_info = fts.extract_factor(last_index);
        std::unique_ptr<MergeAndShrinkRepresentation> state_mapping =
            std::move(factor_info.first);

        State initial = task_proxy_.get_initial_state();
        initial.unpack();

        initial_state_ = QuotientState(state_mapping->get_value(initial));
        dead_end_state_ = QuotientState(transitions_.size());
    } else {
        initial_state_ = QuotientState(0);
        dead_end_state_ = QuotientState(0);
    }

    std::cout << "Rebuilt probabilistic bisimulation in " << timer << std::endl;
    std::cout << "Total time for computing probabilistic bisimulation: "
              << timer_total << std::endl;
}

BisimilarStateSpace::~BisimilarStateSpace() = default;

StateID BisimilarStateSpace::get_state_id(QuotientState s)
{
    return std::to_underlying(s);
}

QuotientState BisimilarStateSpace::get_state(StateID s)
{
    return static_cast<QuotientState>(s.id);
}

void BisimilarStateSpace::generate_applicable_actions(
    QuotientState state,
    std::vector<QuotientAction>& result)
{
    if (state == dead_end_state_) {
        return;
    }

    const std::vector<CachedTransition>& cache =
        transitions_[std::to_underlying(state)];
    result.reserve(cache.size());
    for (unsigned i = 0; i < cache.size(); ++i) {
        result.emplace_back(static_cast<QuotientAction>(i));
    }
}

void BisimilarStateSpace::generate_action_transitions(
    QuotientState state,
    QuotientAction a,
    Distribution<StateID>& result)
{
    assert(state != dead_end_state_);
    assert(
        std::to_underlying(a) <
        static_cast<int>(transitions_[std::to_underlying(state)].size()));

    const ProbabilisticOperatorsProxy operators = task_proxy_.get_operators();

    const CachedTransition& t =
        transitions_[std::to_underlying(state)][std::to_underlying(a)];
    const ProbabilisticOperatorProxy& op = operators[t.op];
    const ProbabilisticOutcomesProxy& outcomes = op.get_outcomes();

    for (unsigned i = 0; i < outcomes.size(); ++i) {
        const ProbabilisticOutcomeProxy outcome = outcomes[i];
        const value_t probability = outcome.get_probability();
        const StateID id = t.successors[i] == PRUNED_STATE
                               ? std::to_underlying(dead_end_state_)
                               : t.successors[i];
        result.add_probability(id, probability);
    }
}

void BisimilarStateSpace::generate_all_transitions(
    QuotientState state,
    std::vector<QuotientAction>& aops,
    std::vector<Distribution<StateID>>& result)
{
    generate_applicable_actions(state, aops);
    result.resize(aops.size());
    for (auto [s_aops, s_transitions] : std::views::zip(aops, result)) {
        generate_action_transitions(state, s_aops, s_transitions);
    }
}

void BisimilarStateSpace::generate_all_transitions(
    QuotientState state,
    std::vector<TransitionType>& transitions)
{
    if (state == dead_end_state_) {
        return;
    }

    const std::vector<CachedTransition>& cache =
        transitions_[std::to_underlying(state)];
    transitions.reserve(cache.size());
    for (unsigned i : std::views::iota(0U, cache.size())) {
        auto a = static_cast<QuotientAction>(i);
        TransitionType& t = transitions.emplace_back(a);
        generate_action_transitions(state, a, t.successor_dist);
    }
}

TerminationInfo BisimilarStateSpace::get_termination_info(QuotientState s)
{
    return is_goal_state(s) ? TerminationInfo::from_goal()
                            : TerminationInfo::from_non_goal(upper_bound_);
}

value_t BisimilarStateSpace::get_action_cost(QuotientAction)
{
    return 0;
}

QuotientState BisimilarStateSpace::get_initial_state() const
{
    return initial_state_;
}

bool BisimilarStateSpace::is_goal_state(QuotientState s) const
{
    return s != dead_end_state_ && goal_flags_[std::to_underlying(s)];
}

bool BisimilarStateSpace::is_dead_end(QuotientState s) const
{
    return s == dead_end_state_;
}

unsigned BisimilarStateSpace::num_bisimilar_states() const
{
    const auto size = goal_flags_.size();
    return size != 0 ? size : 1;
}

unsigned BisimilarStateSpace::num_transitions() const
{
    return num_cached_transitions_;
}

void BisimilarStateSpace::dump(std::ostream& out) const
{
    const ProbabilisticOperatorsProxy operators = task_proxy_.get_operators();

    out << "digraph {" << "\n";

    if (initial_state_ == dead_end_state_) {
        out << "init [shape=ellipse, label=\"dead\"];" << "\n";
        out << "-> init;" << "\n";
        out << "}" << std::flush;
        return;
    }

    for (unsigned node = 0; node < transitions_.size(); ++node) {
        out << "n" << node << " [shape=circle, label=\"#" << node << "\""
            << (goal_flags_[node] ? ", peripheries=2" : "") << "];\n";
    }
    out << "n" << transitions_.size() << " [shape=circle, label=\"dead\"];\n";

    unsigned t = 0;
    for (unsigned node = 0; node < transitions_.size(); ++node) {
        const std::vector<CachedTransition>& ts = transitions_[node];
        for (auto i : ts) {
            out << "t" << t << " [shape=rectangle, label=\""
                << operators[i.op].get_name() << "\"];\n";
            ++t;
        }
    }

    out << "\n";
    out << "\"\" -> n" << std::to_underlying(initial_state_) << "\n";

    t = 0;
    for (unsigned node = 0; node < transitions_.size(); ++node) {
        const std::vector<CachedTransition>& ts = transitions_[node];
        for (unsigned i = 0; i < ts.size(); ++i) {
            out << "n" << node << " -> t" << t << ";\n";
            Distribution<int> succs;
            const ProbabilisticOutcomesProxy outcomes =
                operators[ts[i].op].get_outcomes();
            for (unsigned j = 0; j < outcomes.size(); ++j) {
                const ProbabilisticOutcomeProxy outcome = outcomes[j];
                const int succ = ts[i].successors[j];
                succs.add_probability(
                    succ == PRUNED_STATE ? (int)transitions_.size() : succ,
                    outcome.get_probability());
            }
            for (const auto item : succs.support()) {
                out << "t" << t << " -> n" << item << "\n";
            }
            ++t;
        }
    }

    out << "}" << std::flush;
}

} // namespace bisimulation
} // namespace probfd
