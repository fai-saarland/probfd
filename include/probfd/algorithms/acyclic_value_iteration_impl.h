#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H
#error "This file should only be included from acyclic_value_iteration.h"
#endif

#include "probfd/algorithms/utils.h"

#include "probfd/policies/map_policy.h"

#include "probfd/heuristic.h"
#include "probfd/mdp.h"

#include "downward/utils/countdown_timer.h"

namespace probfd::algorithms::acyclic_vi {

namespace internal {

template <typename State, typename Action>
DFSExplorationState<State, Action>::DFSExplorationState(
    StateID state_id,
    StateInfo<Action>& state_info)
    : state_id(state_id)
    , state_info(state_info)
{
}

template <typename State, typename Action>
void DFSExplorationState<State, Action>::setup_transition(
    MDP<State, Action>& mdp)
{
    assert(successor_dist.non_source_successor_dist.empty());
    auto& next_action = remaining_aops.back();
    t_value = mdp.get_action_cost(next_action);
    const State state = mdp.get_state(state_id);
    mdp.generate_action_transitions(state, next_action, successor_dist);
    successor = successor_dist.non_source_successor_dist.begin();
}

template <typename State, typename Action>
void DFSExplorationState<State, Action>::backtrack_successor(
    value_t probability,
    StateInfo<Action>& succ_info)
{
    // Update transition Q-value
    t_value += probability * succ_info.value;
    succ_info.status = StateInfo<Action>::CLOSED;
}

template <typename State, typename Action>
bool DFSExplorationState<State, Action>::advance(
    MDP<State, Action>& mdp,
    policies::MapPolicy<State, Action>* policy)
{
    return next_successor() || next_transition(mdp, policy);
}

template <typename State, typename Action>
bool DFSExplorationState<State, Action>::next_successor()
{
    assert(successor != successor_dist.non_source_successor_dist.end());
    if (++successor != successor_dist.non_source_successor_dist.end()) {
        return true;
    }

    finalize_transition();

    return false;
}

template <typename State, typename Action>
bool DFSExplorationState<State, Action>::next_transition(
    MDP<State, Action>& mdp,
    policies::MapPolicy<State, Action>* policy)
{
    assert(!remaining_aops.empty());
    remaining_aops.pop_back();

    if (remaining_aops.empty()) {
        finalize_expansion(policy);
        return false;
    }

    successor_dist.clear();
    setup_transition(mdp);

    return true;
}

template <typename State, typename Action>
void DFSExplorationState<State, Action>::finalize_transition()
{
    if (successor_dist.non_source_probability != 0_vt) {
        t_value = (1 / successor_dist.non_source_probability) * t_value;
    }

    // Minimum Q-value
    if (t_value < state_info.value) {
        state_info.best_action = remaining_aops.back();
        state_info.value = t_value;
    }
}

template <typename State, typename Action>
void DFSExplorationState<State, Action>::finalize_expansion(
    policies::MapPolicy<State, Action>* policy)
{
    if (!policy || !state_info.best_action) return;
    policy->emplace_decision(
        state_id,
        *state_info.best_action,
        Interval(state_info.value));
}

} // namespace internal

template <typename State, typename Action>
auto AcyclicValueIteration<State, Action>::compute_policy(
    MDPType& mdp,
    HeuristicType& heuristic,
    ParamType<State> initial_state,
    ProgressReport,
    downward::utils::Duration max_time) -> std::unique_ptr<PolicyType>
{
    std::unique_ptr<MapPolicy> policy(new MapPolicy(&mdp));
    this->solve(mdp, heuristic, initial_state, max_time, policy.get());
    return policy;
}

template <typename State, typename Action>
Interval AcyclicValueIteration<State, Action>::solve(
    MDPType& mdp,
    HeuristicType& heuristic,
    ParamType<State> initial_state,
    ProgressReport,
    downward::utils::Duration max_time)
{
    return solve(mdp, heuristic, initial_state, max_time, nullptr);
}

template <typename State, typename Action>
Interval AcyclicValueIteration<State, Action>::solve(
    MDPType& mdp,
    HeuristicType& heuristic,
    ParamType<State> initial_state,
    downward::utils::Duration max_time,
    MapPolicy* policy)
{
    downward::utils::CountdownTimer timer(max_time);

    const StateID initial_state_id = mdp.get_state_id(initial_state);
    StateInfo& iinfo = state_infos_[initial_state_id];

    dfs_stack_.emplace(initial_state_id, iinfo);
    iinfo.status = StateInfo::ON_STACK;

    DFSExplorationState* e;

    for (;;) {
        do {
            e = &dfs_stack_.top();
        } while (expand_state(mdp, heuristic, *e) &&
                 push_successor(mdp, policy, *e, timer));

        do {
            dfs_stack_.pop();

            if (dfs_stack_.empty()) {
                return Interval(iinfo.value);
            }

            timer.throw_if_expired();

            e = &dfs_stack_.top();

            const auto [succ_id, probability] = *e->successor;
            e->backtrack_successor(probability, state_infos_[succ_id]);
        } while (!e->advance(mdp, policy) ||
                 !push_successor(mdp, policy, *e, timer));
    }
}

template <typename State, typename Action>
bool AcyclicValueIteration<State, Action>::push_successor(
    MDPType& mdp,
    MapPolicy* policy,
    DFSExplorationState& e,
    downward::utils::CountdownTimer& timer)
{
    using namespace downward::utils;

    do {
        timer.throw_if_expired();

        const auto [succ_id, probability] = *e.successor;
        StateInfo& succ_info = state_infos_[succ_id];

        if (succ_info.status == StateInfo::ON_STACK) {
            throw CriticalError("State space is not acyclic!");
        }

        if (succ_info.status == StateInfo::NEW) {
            dfs_stack_.emplace(succ_id, succ_info);
            succ_info.status = StateInfo::ON_STACK;
            return true; // DFS recursion
        }

        assert(succ_info.status == StateInfo::CLOSED);

        e.backtrack_successor(probability, succ_info);
    } while (e.advance(mdp, policy));

    return false;
}

template <typename State, typename Action>
bool AcyclicValueIteration<State, Action>::expand_state(
    MDPType& mdp,
    HeuristicType& heuristic,
    DFSExplorationState& e_info)
{
    const State state = mdp.get_state(e_info.state_id);
    StateInfo& succ_info = e_info.state_info;

    assert(succ_info.status == StateInfo::ON_STACK);

    const TerminationInfo term_info = mdp.get_termination_info(state);
    const value_t term_value = term_info.get_cost();

    succ_info.value = term_value;

    if (term_info.is_goal_state()) {
        notifyEvent(StateExpansion{state});
        notifyEvent(GoalStateExpansion{state});
        return false;
    }

    if (heuristic.evaluate(state) == term_value) {
        notifyEvent(PruneStateExpansion{state});
        return false;
    }

    notifyEvent(StateExpansion{state});

    assert(e_info.remaining_aops.empty());

    mdp.generate_applicable_actions(state, e_info.remaining_aops);
    if (e_info.remaining_aops.empty()) {
        notifyEvent(TerminalStateExpansion{state});
        return false;
    }

    e_info.setup_transition(mdp);

    return true;
}

} // namespace probfd::algorithms::acyclic_vi
