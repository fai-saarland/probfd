#ifndef PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H
#define PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H

#include "probfd/storage/per_state_storage.h"

#include "probfd/distribution.h"
#include "probfd/mdp_algorithm.h"
#include "probfd/transition_tail.h"

#include "probfd/algorithms/events.h"
#include "probfd/utils/observer.h"

#include <stack>

// Forward Declarations
namespace downward::utils {
class CountdownTimer;
}

namespace probfd::policies {
template <typename, typename>
class MapPolicy;
}

/// Namespace dedicated to the acyclic value iteration algorithm.
namespace probfd::algorithms::acyclic_vi {

namespace internal {

template <typename Action>
struct StateInfo {
    enum Status : uint8_t { NEW, ON_STACK, CLOSED };
    Status status = NEW;
    std::optional<Action> best_action = std::nullopt;
    value_t value = -INFINITE_VALUE;
};

template <typename State, typename Action>
struct DFSExplorationState {
    const StateID state_id;
    StateInfo<Action>& state_info;

    // Applicable operators left to expand
    std::vector<Action> remaining_aops;

    // The current transition and transition successor
    SuccessorDistribution successor_dist;
    typename Distribution<StateID>::const_iterator successor;

    // The current transition Q-value
    value_t t_value;

    DFSExplorationState(StateID state_id, StateInfo<Action>& state_info);

    void setup_transition(MDP<State, Action>& mdp);

    void backtrack_successor(value_t probability, StateInfo<Action>& succ_info);

    bool advance(
        MDP<State, Action>& mdp,
        policies::MapPolicy<State, Action>* policy);

private:
    bool next_successor();
    bool next_transition(
        MDP<State, Action>& mdp,
        policies::MapPolicy<State, Action>* policy);

    void finalize_transition();
    void finalize_expansion(policies::MapPolicy<State, Action>* policy);
};

} // namespace internal

/**
 * @brief Implements acyclic Value Iteration.
 *
 * Performs value iteration on acyclic MDPs by performing a depth-first search
 * ending at terminal states and applying Bellman updates during backtracking.
 * Exactly one Bellman update is performed per encountered state.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 *
 * @remark The search algorithm may only be used with acyclic state spaces.
 * The planner aborts if it detects a cycle.
 */
template <typename State, typename Action>
class AcyclicValueIteration
    : public MDPAlgorithm<State, Action>
    , public Observable<
          StateExpansion,
          GoalStateExpansion,
          TerminalStateExpansion,
          PruneStateExpansion> {
    using Base = typename AcyclicValueIteration::MDPAlgorithm;

    using PolicyType = typename Base::PolicyType;
    using MDPType = typename Base::MDPType;
    using HeuristicType = typename Base::HeuristicType;

    using MapPolicy = policies::MapPolicy<State, Action>;

    using StateInfo = internal::StateInfo<Action>;
    using DFSExplorationState = internal::DFSExplorationState<State, Action>;

    storage::PerStateStorage<StateInfo> state_infos_;
    std::stack<DFSExplorationState> dfs_stack_;

public:
    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> initial_state,
        ProgressReport progress,
        downward::utils::Duration max_time) override;

    Interval solve(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> initial_state,
        ProgressReport progress,
        downward::utils::Duration max_time);

private:
    Interval solve(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> initial_state,
        downward::utils::Duration max_time,
        MapPolicy* policy);

    bool push_successor(
        MDPType& mdp,
        MapPolicy* policy,
        DFSExplorationState& e,
        downward::utils::CountdownTimer& timer);

    bool expand_state(
        MDPType& mdp,
        HeuristicType& heuristic,
        DFSExplorationState& e_info);
};

} // namespace probfd::algorithms::acyclic_vi

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H

#include "probfd/algorithms/acyclic_value_iteration_impl.h"

#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H

#endif // PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H