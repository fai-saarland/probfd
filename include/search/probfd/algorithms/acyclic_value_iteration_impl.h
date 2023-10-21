#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H
#error "This file should only be included from acyclic_value_iteration.h"
#endif

#include "probfd/algorithms/utils.h"

#include "probfd/policies/map_policy.h"

#include "probfd/evaluator.h"
#include "probfd/mdp.h"

#include "downward/utils/countdown_timer.h"

namespace probfd {
namespace algorithms {
namespace acyclic_vi {

template <typename State, typename Action>
AcyclicValueIteration<State, Action>::IncrementalExpansionInfo::
    IncrementalExpansionInfo(
        StateID state_id,
        std::vector<Action> remaining_aops,
        MDP& mdp)
    : state_id(state_id)
    , remaining_aops(std::move(remaining_aops))
{
    assert(!this->remaining_aops.empty());
    setup_transition(mdp);
}

template <typename State, typename Action>
bool AcyclicValueIteration<State, Action>::IncrementalExpansionInfo::
    next_successor()
{
    assert(successor != transition.end());
    return ++successor != transition.end();
}

template <typename State, typename Action>
bool AcyclicValueIteration<State, Action>::IncrementalExpansionInfo::
    next_transition(MDP& mdp)
{
    assert(!remaining_aops.empty());
    remaining_aops.pop_back();

    if (remaining_aops.empty()) {
        return false;
    }

    transition.clear();
    setup_transition(mdp);

    return true;
}

template <typename State, typename Action>
void AcyclicValueIteration<State, Action>::IncrementalExpansionInfo::
    setup_transition(MDP& mdp)
{
    auto& next_action = remaining_aops.back();
    t_value = mdp.get_action_cost(next_action);
    const State state = mdp.get_state(state_id);
    mdp.generate_action_transitions(state, next_action, transition);
    successor = transition.begin();
}

template <typename State, typename Action>
auto AcyclicValueIteration<State, Action>::compute_policy(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> initial_state,
    ProgressReport,
    double max_time) -> std::unique_ptr<Policy>
{
    std::unique_ptr<MapPolicy> policy(new MapPolicy(&mdp));
    this->solve(mdp, heuristic, initial_state, max_time, policy.get());
    return policy;
}

template <typename State, typename Action>
Interval AcyclicValueIteration<State, Action>::solve(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> initial_state,
    ProgressReport,
    double max_time)
{
    return solve(mdp, heuristic, initial_state, max_time, nullptr);
}

template <typename State, typename Action>
Interval AcyclicValueIteration<State, Action>::solve(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> initial_state,
    double max_time,
    MapPolicy* policy)
{
    utils::CountdownTimer timer(max_time);

    const StateID initial_state_id = mdp.get_state_id(initial_state);
    StateInfo& iinfo = state_infos_[initial_state_id];

    if (!push_state(mdp, heuristic, initial_state_id, iinfo)) {
        return Interval(iinfo.value);
    }

    do {
        dfs_expand(mdp, heuristic, timer, policy);
    } while (dfs_backtrack(mdp, timer, policy));

    assert(expansion_stack_.empty());
    return Interval(iinfo.value);
}

template <typename State, typename Action>
void AcyclicValueIteration<State, Action>::print_statistics(
    std::ostream& out) const
{
    statistics_.print(out);
}

template <typename State, typename Action>
void AcyclicValueIteration<State, Action>::dfs_expand(
    MDP& mdp,
    Evaluator& heuristic,
    utils::CountdownTimer& timer,
    MapPolicy* policy)
{
    IncrementalExpansionInfo* e = &expansion_stack_.top();

    do {
        do {
            timer.throw_if_expired();

            const auto [succ_id, probability] = *e->successor;
            StateInfo& succ_info = state_infos_[succ_id];

            if (!succ_info.expanded &&
                push_state(mdp, heuristic, succ_id, succ_info)) {
                e = &expansion_stack_.top();
                continue; // DFS recursion
            }

            backtrack_successor(*e, probability, succ_info);
        } while (e->next_successor());

        finalize_transition(*e);
    } while (e->next_transition(mdp));

    finalize_expansion(*e, policy);
}

template <typename State, typename Action>
void AcyclicValueIteration<State, Action>::backtrack_successor(
    IncrementalExpansionInfo& e,
    value_t probability,
    StateInfo& succ_info)
{
    // Update transition Q-value
    e.t_value += probability * succ_info.value;
}

template <typename State, typename Action>
void AcyclicValueIteration<State, Action>::finalize_transition(
    IncrementalExpansionInfo& e)
{
    // Minimum Q-value
    StateInfo& info = state_infos_[e.state_id];

    if (e.t_value < info.value) {
        info.best_action = e.remaining_aops.back();
        info.value = e.t_value;
    }
}

template <typename State, typename Action>
void AcyclicValueIteration<State, Action>::finalize_expansion(
    IncrementalExpansionInfo& e,
    MapPolicy* policy)
{
    StateInfo& info = state_infos_[e.state_id];
    if (!policy) return;
    policy->emplace_decision(
        e.state_id,
        *info.best_action,
        Interval(info.value));
}

template <typename State, typename Action>
bool AcyclicValueIteration<State, Action>::dfs_backtrack(
    MDP& mdp,
    utils::CountdownTimer& timer,
    MapPolicy* policy)
{
    IncrementalExpansionInfo* e;

    for (;;) {
        expansion_stack_.pop();

        if (expansion_stack_.empty()) {
            return false;
        }

        timer.throw_if_expired();

        e = &expansion_stack_.top();

        const auto [succ_id, probability] = *e->successor;
        backtrack_successor(*e, probability, state_infos_[succ_id]);

        if (e->next_successor()) {
            return true;
        }

        finalize_transition(*e);

        if (e->next_transition(mdp)) {
            return true;
        }

        finalize_expansion(*e, policy);
    }
}

template <typename State, typename Action>
bool AcyclicValueIteration<State, Action>::push_state(
    MDP& mdp,
    Evaluator& heuristic,
    StateID state_id,
    StateInfo& succ_info)
{
    assert(!succ_info.expanded);
    succ_info.expanded = true;

    const State state = mdp.get_state(state_id);
    const TerminationInfo term_info = mdp.get_termination_info(state);
    const value_t term_value = term_info.get_cost();

    succ_info.value = term_value;

    if (term_info.is_goal_state()) {
        ++statistics_.terminal_states;
        ++statistics_.goal_states;
        return false;
    }

    if (heuristic.evaluate(state) == term_value) {
        ++statistics_.pruned;
        return false;
    }

    std::vector<Action> remaining_aops;
    mdp.generate_applicable_actions(state, remaining_aops);
    if (remaining_aops.empty()) {
        ++statistics_.terminal_states;
        return false;
    }

    ++statistics_.state_expansions;
    expansion_stack_.emplace(state_id, std::move(remaining_aops), mdp);

    return true;
}

} // namespace acyclic_vi
} // namespace algorithms
} // namespace probfd