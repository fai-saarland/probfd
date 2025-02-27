#ifndef PROBFD_BISIMULATION_BISIMILAR_STATE_SPACE_H
#define PROBFD_BISIMULATION_BISIMILAR_STATE_SPACE_H

#include "probfd/bisimulation/types.h"

#include "probfd/fdr_types.h"
#include "probfd/mdp.h"

#include "probfd/state_id.h"
#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include "downward/algorithms/segmented_vector.h"

#include <iosfwd>
#include <memory>
#include <vector>

// Forward Declarations
namespace merge_and_shrink {
class Distances;
class FactoredTransitionSystem;
class TransitionSystem;
struct Factor;
} // namespace merge_and_shrink

namespace probfd {
template <typename>
class Distribution;
class ProbabilisticTask;
} // namespace probfd

namespace probfd::bisimulation {

class BisimilarStateSpace : public MDP<QuotientState, OperatorID> {
    struct CachedTransition {
        OperatorID op_id;
        int* successors;
    };

    std::shared_ptr<ProbabilisticTask> task_;
    std::shared_ptr<FDRCostFunction> task_cost_function_;

    unsigned num_cached_transitions_ = 0;
    segmented_vector::SegmentedVector<std::vector<CachedTransition>>
        transitions_;

    // Storage for transitions
    std::vector<std::unique_ptr<int[]>> store_;

    std::vector<bool> goal_flags_;

public:
    BisimilarStateSpace(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        const merge_and_shrink::TransitionSystem& transition_system);

    ~BisimilarStateSpace() override;

    StateID get_state_id(QuotientState state) override;

    QuotientState get_state(StateID state_id) override;

    void generate_applicable_actions(
        QuotientState state,
        std::vector<OperatorID>& result) override;

    void generate_action_transitions(
        QuotientState state,
        OperatorID action,
        SuccessorDistribution& successor_dist) override;

    void generate_all_transitions(
        QuotientState state,
        std::vector<OperatorID>& aops,
        std::vector<SuccessorDistribution>& successor_dists) override;

    void generate_all_transitions(
        QuotientState state,
        std::vector<TransitionTailType>& transitions) override;

    TerminationInfo get_termination_info(QuotientState state) override;

    value_t get_action_cost(OperatorID action) override;

    /// Checks whether the given quotient state is a goal state.
    bool is_goal_state(QuotientState s) const;

    /// Gets the number of states in the probabilistic bisimulation.
    unsigned num_bisimilar_states() const;

    /// Gets the number of transitions in the probabilistic bisimulation.
    unsigned num_transitions() const;
};

merge_and_shrink::Factor
compute_bisimulation_on_determinization(const TaskProxy& det_task_proxy);

} // namespace probfd::bisimulation

#endif // PROBFD_BISIMULATION_BISIMILAR_STATE_SPACE_H