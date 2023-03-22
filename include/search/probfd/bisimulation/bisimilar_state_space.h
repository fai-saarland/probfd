#ifndef PROBFD_BISIMULATION_BISIMILAR_STATE_SPACE_H
#define PROBFD_BISIMULATION_BISIMILAR_STATE_SPACE_H

#include "algorithms/segmented_vector.h"

#include "probfd/bisimulation/types.h"

#include "probfd/distribution.h"
#include "probfd/types.h"

#include "probfd/task_proxy.h"

#include "probfd/engine_interfaces/state_space.h"

#include <cassert>
#include <memory>
#include <unordered_set>
#include <vector>

class State;

namespace merge_and_shrink {
class Distances;
class FactoredTransitionSystem;
class TransitionSystem;
} // namespace merge_and_shrink

namespace probfd {
namespace bisimulation {

/**
 * @brief Implements a probabilistic bisimulation of the task's state space
 * using the classical Merge-and-Shrink algorithm on the determinization.
 *
 * This implementation first runs the classical merge-and-shrink algorithm on
 * the all-outcomes determinization of a planning task, using bisimulation-based
 * exact shrinking and without label reduction. The resulting state mapping of
 * the abstraction specifies a bisimulation relation on the determinization,
 * which is also probabilistic bisimulation (although usually not the coarsest)
 * on the original state space. The quotient is constructed with respect to this
 * probabilistic bisimulation.
 */
class BisimilarStateSpace
    : public engine_interfaces::StateSpace<
          bisimulation::QuotientState,
          bisimulation::QuotientAction> {
    struct CachedTransition {
        unsigned op;
        int* successors;
    };

public:
    /**
     * @brief Constructs the quotient of the induced state space of the task
     * with respect to a bisimulation of the all outcomes determinization.
     */
    explicit BisimilarStateSpace(const ProbabilisticTask* task);
    ~BisimilarStateSpace();

    /// Get the initial state of the probabilistic bisimulation quotient.
    QuotientState get_initial_state() const;

    /// Returns true iff the given quotient state is a goal.
    bool is_goal_state(const QuotientState& s) const;

    /// Returns true iff the given quotient state is a dead end.
    bool is_dead_end(const QuotientState& s) const;

    StateID get_state_id(bisimulation::QuotientState state) override;
    bisimulation::QuotientState get_state(StateID state_id) override;

    ActionID
    get_action_id(StateID state_id, bisimulation::QuotientAction action)
        override;

    bisimulation::QuotientAction
    get_action(StateID state_id, ActionID action) override;

    void generate_applicable_actions(
        StateID state,
        std::vector<bisimulation::QuotientAction>& result) override;
    void generate_action_transitions(
        StateID state,
        bisimulation::QuotientAction action,
        Distribution<StateID>& result) override;
    void generate_all_transitions(
        StateID state,
        std::vector<bisimulation::QuotientAction>& aops,
        std::vector<Distribution<StateID>>& result) override;

    /// Returns the number of states in the probabilistic bisimulation.
    unsigned num_bisimilar_states() const;

    /// Returns the number of transitions in the probabilistic bisimulation.
    unsigned num_transitions() const;

    /// Dumps the quotient state space as a dot graph.
    void dump(std::ostream& out) const;

private:
    ProbabilisticTaskProxy task_proxy;

    std::unique_ptr<merge_and_shrink::FactoredTransitionSystem> fts_;
    const merge_and_shrink::TransitionSystem* abstraction_;
    std::unique_ptr<merge_and_shrink::Distances> distances_;
    unsigned num_cached_transitions_;

    QuotientState initial_state_;
    QuotientState dead_end_state_;
    segmented_vector::SegmentedVector<std::vector<CachedTransition>>
        transitions_;

    std::vector<std::unique_ptr<int[]>> store_;
};

} // namespace bisimulation
} // namespace probfd

#endif // __BISIMILAR_STATE_SPACE_H__