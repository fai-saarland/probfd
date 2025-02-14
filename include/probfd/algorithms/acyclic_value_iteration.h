#ifndef PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H
#define PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H

#include "probfd/storage/per_state_storage.h"

#include "probfd/distribution.h"
#include "probfd/mdp_algorithm.h"
#include "probfd/transition_tail.h"

#include <stack>

// Forward Declarations
namespace utils {
class CountdownTimer;
}

namespace probfd::policies {
template <typename, typename>
class MapPolicy;
}

/// Namespace dedicated to the acyclic value iteration algorithm.
namespace probfd::algorithms::acyclic_vi {

/**
 * @brief Models an observer subscribed to events of the acyclic value
 * iteration algorithm.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The state type of the underlying MDP model.
 */
template <typename State, typename Action>
class AcyclicValueIterationObserver {
public:
    virtual ~AcyclicValueIterationObserver() = default;

    /// Called when the algorithm selects a state for expansion.
    virtual void on_state_selected_for_expansion(const State&) = 0;

    /// Called when a goal state is encountered during the expansion check.
    virtual void on_goal_state(const State&) = 0;

    /// Called when a terminal state is encountered during the expansion check.
    virtual void on_terminal_state(const State&) = 0;

    /// Called when a state is pruned during the expansion check.
    virtual void on_pruned_state(const State&) = 0;
};

/**
 * @brief An observer that collects basic statistics of the acyclic value
 * iteration algorithm.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The state type of the underlying MDP model.
 */
template <typename State, typename Action>
struct StatisticsObserver
    : public AcyclicValueIterationObserver<State, Action> {
    unsigned long long state_expansions = 0;
    unsigned long long terminal_states = 0;
    unsigned long long goal_states = 0;
    unsigned long long pruned_states = 0;

    void on_state_selected_for_expansion(const State&);
    void on_goal_state(const State&);
    void on_terminal_state(const State&);
    void on_pruned_state(const State&);

    void print(std::ostream& out) const;
};

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

template <typename State, typename Action>
class AcyclicValueIterationObserverCollection {
    using Observer = AcyclicValueIterationObserver<State, Action>;

    std::vector<Observer> observers_;

public:
    void register_observer(std::shared_ptr<Observer> observer);

    void notify_state_selected_for_expansion(const State&);
    void notify_goal_state(const State&);
    void notify_terminal_state(const State&);
    void notify_pruned_state(const State&);
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
class AcyclicValueIteration : public MDPAlgorithm<State, Action> {
    using Base = typename AcyclicValueIteration::MDPAlgorithm;

    using PolicyType = typename Base::PolicyType;
    using MDPType = typename Base::MDPType;
    using HeuristicType = typename Base::HeuristicType;

    using MapPolicy = policies::MapPolicy<State, Action>;

    using StateInfo = internal::StateInfo<Action>;
    using DFSExplorationState = internal::DFSExplorationState<State, Action>;

    using Observer = AcyclicValueIterationObserver<State, Action>;
    using ObserverCollection =
        internal::AcyclicValueIterationObserverCollection<State, Action>;

    storage::PerStateStorage<StateInfo> state_infos_;
    std::stack<DFSExplorationState> dfs_stack_;

    ObserverCollection observers_;

public:
    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> initial_state,
        ProgressReport progress,
        double max_time) override;

    Interval solve(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> initial_state,
        ProgressReport progress,
        double max_time) override;

    Interval solve(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> initial_state,
        double max_time,
        MapPolicy* policy);

    void register_observer(std::shared_ptr<Observer> observer);

private:
    bool push_successor(
        MDPType& mdp,
        MapPolicy* policy,
        DFSExplorationState& e,
        utils::CountdownTimer& timer);

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