#ifndef PROBFD_BISIMULATION_BISIMILAR_STATE_SPACE_H
#define PROBFD_BISIMULATION_BISIMILAR_STATE_SPACE_H

#include "probfd/bisimulation/types.h"

#include "probfd/mdp.h"

#include "probfd/task_proxy.h"
#include "probfd/types.h"
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

class BisimilarStateSpace : public MDP<QuotientState, QuotientAction> {
    struct CachedTransition {
        unsigned op;
        int* successors;
    };

    std::shared_ptr<ProbabilisticTask> task_;
    const value_t upper_bound_;

    std::vector<bool> goal_flags_;

    QuotientState dead_end_state_;

    unsigned num_cached_transitions_;
    segmented_vector::SegmentedVector<std::vector<CachedTransition>>
        transitions_;
    std::vector<std::unique_ptr<int[]>> store_;

public:
    BisimilarStateSpace(
        std::shared_ptr<ProbabilisticTask> task,
        const TaskProxy& det_task_proxy,
        const merge_and_shrink::TransitionSystem& transition_system,
        value_t upper_bound);

    ~BisimilarStateSpace() override;

    StateID get_state_id(QuotientState state) override;

    QuotientState get_state(StateID state_id) override;

    void generate_applicable_actions(
        QuotientState state,
        std::vector<QuotientAction>& result) override;

    void generate_action_transitions(
        QuotientState state,
        QuotientAction action,
        Distribution<StateID>& result) override;

    void generate_all_transitions(
        QuotientState state,
        std::vector<QuotientAction>& aops,
        std::vector<Distribution<StateID>>& result) override;

    void generate_all_transitions(
        QuotientState state,
        std::vector<TransitionType>& transitions) override;

    TerminationInfo get_termination_info(QuotientState state) override;

    value_t get_action_cost(QuotientAction action) override;

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