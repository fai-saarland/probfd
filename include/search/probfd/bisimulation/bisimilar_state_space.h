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

/**
 * @brief Implements a probabilistic bisimulation of the task's state space
 * using the classical Merge-and-Shrink algorithm on the determinization.
 *
 * This implementation first runs the classical merge-and-shrink algorithm on
 * the all-outcomes determinization of a planning task, using bisimulation-based
 * exact shrinking and without label reduction. The resulting state mapping of
 * the abstraction specifies a bisimulation relation on the determinization,
 * which is also a probabilistic bisimulation relation on the original state
 * space (although usually not the coarsest). The quotient is constructed with
 * respect to this probabilistic bisimulation.
 */
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
    /**
     * @brief Constructs the quotient of the induced state space of the task
     * with respect to a bisimulation of the all outcomes determinization.
     */
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

    /// Checks whether the given quotient state is a dead end.
    bool is_dead_end(QuotientState s) const;

    /// Gets the number of states in the probabilistic bisimulation.
    unsigned num_bisimilar_states() const;

    /// Gets the number of transitions in the probabilistic bisimulation.
    unsigned num_transitions() const;

    /// Dumps the quotient state space to an output stream as a dot graph.
    void dump(std::ostream& out) const;
};

merge_and_shrink::Factor
compute_bisimulation_on_determinization(const TaskProxy& det_task_proxy);

} // namespace probfd::bisimulation

#endif // PROBFD_BISIMULATION_BISIMILAR_STATE_SPACE_H