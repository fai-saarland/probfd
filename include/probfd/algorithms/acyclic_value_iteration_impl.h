#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H
#error "This file should only be included from acyclic_value_iteration.h"
#endif

#include "probfd/algorithms/utils.h"

#include "probfd/policies/map_policy.h"

#include "probfd/evaluator.h"
#include "probfd/mdp.h"

#include "downward/utils/countdown_timer.h"

namespace probfd::algorithms::acyclic_vi {

template <typename State, typename Action>
void StatisticsObserver<State, Action>::on_state_selected_for_expansion(
    const State&)
{
    ++state_expansions;
}

template <typename State, typename Action>
void StatisticsObserver<State, Action>::on_goal_state(const State&)
{
    ++goal_states;
}

template <typename State, typename Action>
void StatisticsObserver<State, Action>::on_terminal_state(const State&)
{
    ++terminal_states;
}

template <typename State, typename Action>
void StatisticsObserver<State, Action>::on_pruned_state(const State&)
{
    ++pruned_states;
}

template <typename State, typename Action>
void StatisticsObserver<State, Action>::print(std::ostream& out) const
{
    out << "  Expanded state(s): " << state_expansions << std::endl;
    out << "  Pruned state(s): " << pruned_states << std::endl;
    out << "  Terminal state(s): " << terminal_states << std::endl;
    out << "  Goal state(s): " << goal_states << std::endl;
}

namespace internal {

template <typename State, typename Action>
void AcyclicValueIterationObserverCollection<State, Action>::register_observer(
    std::shared_ptr<Observer> observer)
{
    observers_.push_back(std::move(observer));
}

template <typename State, typename Action>
void AcyclicValueIterationObserverCollection<State, Action>::
    notify_state_selected_for_expansion(const State& state)
{
    for (auto& observer : observers_) {
        observer.on_state_selected_for_expansion(state);
    }
}

template <typename State, typename Action>
void AcyclicValueIterationObserverCollection<State, Action>::notify_goal_state(
    const State& state)
{
    for (auto& observer : observers_) {
        observer.on_goal_state(state);
    }
}

template <typename State, typename Action>
void AcyclicValueIterationObserverCollection<State, Action>::
    notify_terminal_state(const State& state)
{
    for (auto& observer : observers_) {
        observer.on_terminal_state(state);
    }
}

template <typename State, typename Action>
void AcyclicValueIterationObserverCollection<State, Action>::
    notify_pruned_state(const State& state)
{
    for (auto& observer : observers_) {
        observer.on_pruned_state(state);
    }
}

} // namespace internal

template <typename State, typename Action>
AcyclicValueIteration<State, Action>::IncrementalExpansionInfo::
    IncrementalExpansionInfo(StateID state_id, StateInfo& state_info)
    : state_id(state_id)
    , state_info(state_info)
{
}

template <typename State, typename Action>
void AcyclicValueIteration<State, Action>::IncrementalExpansionInfo::
    setup_transition(MDPType& mdp)
{
    assert(transition.empty());
    auto& next_action = remaining_aops.back();
    t_value = mdp.get_action_cost(next_action);
    const State state = mdp.get_state(state_id);
    mdp.generate_action_transitions(state, next_action, transition);
    successor = transition.begin();
}

template <typename State, typename Action>
void AcyclicValueIteration<State, Action>::IncrementalExpansionInfo::
    backtrack_successor(value_t probability, StateInfo& succ_info)
{
    // Update transition Q-value
    t_value += probability * succ_info.value;
    succ_info.status = StateInfo::CLOSED;
}

template <typename State, typename Action>
bool AcyclicValueIteration<State, Action>::IncrementalExpansionInfo::advance(
    MDPType& mdp,
    MapPolicy* policy)
{
    return next_successor() || next_transition(mdp, policy);
}

template <typename State, typename Action>
bool AcyclicValueIteration<State, Action>::IncrementalExpansionInfo::
    next_successor()
{
    assert(successor != transition.end());
    if (++successor != transition.end()) {
        return true;
    }

    finalize_transition();

    return false;
}

template <typename State, typename Action>
bool AcyclicValueIteration<State, Action>::IncrementalExpansionInfo::
    next_transition(MDPType& mdp, MapPolicy* policy)
{
    assert(!remaining_aops.empty());
    remaining_aops.pop_back();

    if (remaining_aops.empty()) {
        finalize_expansion(policy);
        return false;
    }

    transition.clear();
    setup_transition(mdp);

    return true;
}

template <typename State, typename Action>
void AcyclicValueIteration<State, Action>::IncrementalExpansionInfo::
    finalize_transition()
{
    // Minimum Q-value
    if (t_value < state_info.value) {
        state_info.best_action = remaining_aops.back();
        state_info.value = t_value;
    }
}

template <typename State, typename Action>
void AcyclicValueIteration<State, Action>::IncrementalExpansionInfo::
    finalize_expansion(MapPolicy* policy)
{
    if (!policy || !state_info.best_action) return;
    policy->emplace_decision(
        state_id,
        *state_info.best_action,
        Interval(state_info.value));
}

template <typename State, typename Action>
auto AcyclicValueIteration<State, Action>::compute_policy(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> initial_state,
    ProgressReport,
    double max_time) -> std::unique_ptr<PolicyType>
{
    std::unique_ptr<MapPolicy> policy(new MapPolicy(&mdp));
    this->solve(mdp, heuristic, initial_state, max_time, policy.get());
    return policy;
}

template <typename State, typename Action>
Interval AcyclicValueIteration<State, Action>::solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> initial_state,
    ProgressReport,
    double max_time)
{
    return solve(mdp, heuristic, initial_state, max_time, nullptr);
}

template <typename State, typename Action>
Interval AcyclicValueIteration<State, Action>::solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> initial_state,
    double max_time,
    MapPolicy* policy)
{
    utils::CountdownTimer timer(max_time);

    const StateID initial_state_id = mdp.get_state_id(initial_state);
    StateInfo& iinfo = state_infos_[initial_state_id];

    expansion_stack_.emplace(initial_state_id, iinfo);
    iinfo.status = StateInfo::ON_STACK;

    IncrementalExpansionInfo* e;

    for (;;) {
        do {
            e = &expansion_stack_.top();
        } while (expand_state(mdp, heuristic, *e) &&
                 push_successor(mdp, policy, *e, timer));

        do {
            expansion_stack_.pop();

            if (expansion_stack_.empty()) {
                return Interval(iinfo.value);
            }

            timer.throw_if_expired();

            e = &expansion_stack_.top();

            const auto [succ_id, probability] = *e->successor;
            e->backtrack_successor(probability, state_infos_[succ_id]);
        } while (!e->advance(mdp, policy) ||
                 !push_successor(mdp, policy, *e, timer));
    }
}

template <typename State, typename Action>
void AcyclicValueIteration<State, Action>::register_observer(
    std::shared_ptr<Observer> observer)
{
    observers_.register_observer(std::move(observer));
}

template <typename State, typename Action>
bool AcyclicValueIteration<State, Action>::push_successor(
    MDPType& mdp,
    MapPolicy* policy,
    IncrementalExpansionInfo& e,
    utils::CountdownTimer& timer)
{
    do {
        timer.throw_if_expired();

        const auto [succ_id, probability] = *e.successor;
        StateInfo& succ_info = state_infos_[succ_id];

        if (succ_info.status == StateInfo::ON_STACK) {
            std::cerr << "State space is not acyclic!" << std::endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }

        if (succ_info.status == StateInfo::NEW) {
            expansion_stack_.emplace(succ_id, succ_info);
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
    EvaluatorType& heuristic,
    IncrementalExpansionInfo& e_info)
{
    const State state = mdp.get_state(e_info.state_id);
    StateInfo& succ_info = e_info.state_info;

    assert(succ_info.status == StateInfo::ON_STACK);

    const TerminationInfo term_info = mdp.get_termination_info(state);
    const value_t term_value = term_info.get_cost();

    succ_info.value = term_value;

    if (term_info.is_goal_state()) {
        observers_.notify_state_selected_for_expansion(state);
        observers_.notify_goal_state(state);
        return false;
    }

    if (heuristic.evaluate(state) == term_value) {
        observers_.notify_pruned_state(state);
        return false;
    }

    observers_.notify_state_selected_for_expansion(state);

    assert(e_info.remaining_aops.empty());

    mdp.generate_applicable_actions(state, e_info.remaining_aops);
    if (e_info.remaining_aops.empty()) {
        observers_.notify_terminal_state(state);
        return false;
    }

    e_info.setup_transition(mdp);

    return true;
}

} // namespace probfd::algorithms::acyclic_vi
