#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/distribution.h"
#include "probfd/transition_tail.h"

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

#include "downward/state.h"
#include "probfd/probabilistic_operator_space.h"
#include "probfd/termination_costs.h"

#include <cassert>
#include <limits>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>

using namespace downward;
using namespace downward::merge_and_shrink;

namespace probfd {
class ProbabilisticTask;

namespace bisimulation {

static constexpr int BUCKET_SIZE = 1024 * 64;

BisimilarStateSpace::BisimilarStateSpace(
    SharedProbabilisticTask task,
    const TransitionSystem& transition_system)
    : task_(std::move(task))
    , num_cached_transitions_(0)
    , transitions_(transition_system.get_size() + 1)
    , goal_flags_(transition_system.get_size() + 1, false)
{
    int dead_end_state = transition_system.get_size();

    const auto& prob_operators = get_operators(task_);

    std::vector<std::pair<OperatorID, unsigned>> det_to_prob_op;
    for (unsigned p_op_id = 0; p_op_id < prob_operators.size(); ++p_op_id) {
        const ProbabilisticOperatorProxy op = prob_operators[p_op_id];
        const ProbabilisticOutcomesProxy outcomes = op.get_outcomes();
        for (unsigned i = 0; i < outcomes.size(); ++i) {
            det_to_prob_op.emplace_back(p_op_id, i);
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

    for (const LocalLabelInfo& local_info : transition_system) {
        for (const int det_op_id : local_info.get_label_group()) {
            for (const auto& trans : local_info.get_transitions()) {
                std::vector<CachedTransition>& ts = transitions_[trans.src];
                assert(trans.target != PRUNED_STATE);
                const auto& [id, outcome_index] = det_to_prob_op[det_op_id];

                auto it = std::ranges::find(ts, id, &CachedTransition::op_id);

                if (it == ts.end()) {
                    const int size = prob_operators[id].get_outcomes().size();
                    it = ts.emplace(it, id, allocate(size));
                    for (int j = 0; j != size; ++j) {
                        it->successors[j] = dead_end_state;
                    }
                    ++num_cached_transitions_;
                }

                it->successors[outcome_index] = trans.target;
            }
        }
    }

    for (size_t i = 0; i != transitions_.size(); ++i) {
        auto& ct = transitions_[i];
        std::ranges::sort(ct, {}, &CachedTransition::op_id);
    }

    for (int i = 0; i != transition_system.get_size(); ++i) {
        goal_flags_[i] = transition_system.is_goal_state(i);
    }
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
    std::vector<OperatorID>& result)
{
    const auto& cache = transitions_[std::to_underlying(state)];
    result.reserve(cache.size());
    for (const auto& t : cache) { result.emplace_back(t.op_id); }
}

void BisimilarStateSpace::generate_action_transitions(
    QuotientState state,
    OperatorID a,
    SuccessorDistribution& successor_dist)
{
    const auto& operators = get_operators(task_);

    const auto& transitions = transitions_[std::to_underlying(state)];

    assert(std::ranges::is_sorted(transitions, {}, &CachedTransition::op_id));

    const auto it =
        std::ranges::lower_bound(transitions, a, {}, &CachedTransition::op_id);

    const ProbabilisticOperatorProxy& op = operators[a];
    const ProbabilisticOutcomesProxy& outcomes = op.get_outcomes();

    successor_dist.non_source_probability = 0_vt;

    for (unsigned i = 0; i < outcomes.size(); ++i) {
        const ProbabilisticOutcomeProxy outcome = outcomes[i];
        const value_t probability = outcome.get_probability();
        const StateID id = it->successors[i];
        if (std::to_underlying(state) == static_cast<int>(id)) continue;
        successor_dist.add_non_source_probability(id, probability);
    }
}

void BisimilarStateSpace::generate_all_transitions(
    QuotientState state,
    std::vector<OperatorID>& aops,
    std::vector<SuccessorDistribution>& successor_dists)
{
    const auto& operators = get_operators(task_);

    const auto& cache = transitions_[std::to_underlying(state)];
    aops.reserve(cache.size());
    successor_dists.reserve(cache.size());

    for (const auto [op_id, successors] : cache) {
        aops.push_back(op_id);
        SuccessorDistribution& successor_dist = successor_dists.emplace_back();

        const ProbabilisticOperatorProxy& op = operators[op_id];
        const ProbabilisticOutcomesProxy& outcomes = op.get_outcomes();

        successor_dist.non_source_probability = 0_vt;

        for (unsigned i = 0; i < outcomes.size(); ++i) {
            const ProbabilisticOutcomeProxy outcome = outcomes[i];
            const value_t probability = outcome.get_probability();
            const StateID id = successors[i];
            if (std::to_underlying(state) == static_cast<int>(id)) continue;
            successor_dist.add_non_source_probability(id, probability);
        }
    }
}

void BisimilarStateSpace::generate_all_transitions(
    QuotientState state,
    std::vector<TransitionTailType>& transitions)
{
    const auto& operators = get_operators(task_);

    const auto& cache = transitions_[std::to_underlying(state)];
    transitions.reserve(cache.size());

    for (const auto [op_id, successors] : cache) {
        TransitionTailType& t = transitions.emplace_back(op_id);

        const ProbabilisticOperatorProxy& op = operators[op_id];
        const ProbabilisticOutcomesProxy& outcomes = op.get_outcomes();

        t.successor_dist.non_source_probability = 0_vt;

        for (unsigned i = 0; i < outcomes.size(); ++i) {
            const ProbabilisticOutcomeProxy outcome = outcomes[i];
            const value_t probability = outcome.get_probability();
            const StateID id = successors[i];
            if (std::to_underlying(state) == static_cast<int>(id)) continue;
            t.successor_dist.add_non_source_probability(id, probability);
        }
    }
}

TerminationInfo BisimilarStateSpace::get_termination_info(QuotientState s)
{
    const auto& term_costs = get_termination_costs(task_);

    return is_goal_state(s) ? TerminationInfo::from_goal(
                                  term_costs.get_goal_termination_cost())
                            : TerminationInfo::from_non_goal(
                                  term_costs.get_non_goal_termination_cost());
}

value_t BisimilarStateSpace::get_action_cost(OperatorID op_id)
{
    const auto& cost_function =
        get_cost_function(task_);
    return cost_function.get_operator_cost(op_id.get_index());
}

bool BisimilarStateSpace::is_goal_state(QuotientState s) const
{
    return goal_flags_[std::to_underlying(s)];
}

unsigned BisimilarStateSpace::num_bisimilar_states() const
{
    return transitions_.size();
}

unsigned BisimilarStateSpace::num_transitions() const
{
    return num_cached_transitions_;
}

merge_and_shrink::Factor
compute_bisimulation_on_determinization(const AbstractTaskTuple& det_task)
{
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
        utils::Duration::max(),
        utils::Verbosity::SILENT);

    FactoredTransitionSystem fts =
        mns_algorithm.build_factored_transition_system(det_task);

    assert(fts.get_num_active_entries() == 1);

    return fts.extract_factor(fts.get_size() - 1);
}

} // namespace bisimulation
} // namespace probfd
