#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H
#error "This file should only be included from heuristic_search_base.h"
#endif

#include "probfd/algorithms/policy_picker.h"

#include "probfd/algorithms/utils.h"

#include "probfd/policies/map_policy.h"

#include "probfd/utils/language.h"

#include "probfd/heuristic.h"
#include "probfd/mdp.h"
#include "probfd/transition.h"

#include "downward/utils/collections.h"

#include <cassert>
#include <deque>

namespace probfd::algorithms::heuristic_search {

namespace internal {

inline void Statistics::print(std::ostream& out) const
{
    out << "  Initial state value estimation: " << initial_state_estimate
        << std::endl;
    out << "  Initial state value found terminal: "
        << initial_state_found_terminal << std::endl;

    out << "  Evaluated state(s): " << evaluated_states << std::endl;
    out << "  Pruned state(s): " << pruned_states << std::endl;
    out << "  Goal state(s): " << goal_states << std::endl;
    out << "  Terminal state(s): " << terminal_states << std::endl;
    out << "  Self-loop state(s): " << self_loop_states << std::endl;
    out << "  Expanded state(s): " << expanded_states << std::endl;
    out << "  Number of value updates: " << value_updates << std::endl;
    out << "  Number of value changes: " << value_changes << std::endl;
    out << "  Number of policy updates: " << policy_updates << std::endl;
    out << "  Number of policy changes: " << policy_changes << std::endl;

#if defined(EXPENSIVE_STATISTICS)
    out << "  Updating time: " << update_time << std::endl;
    out << "  Policy selection time: " << policy_selection_time << std::endl;
#endif
}

} // namespace internal

template <typename State, typename Action, typename StateInfoT>
HeuristicSearchBase<State, Action, StateInfoT>::HeuristicSearchBase(
    std::shared_ptr<PolicyPickerType> policy_chooser)
    : policy_chooser_(policy_chooser)
{
}

template <typename State, typename Action, typename StateInfoT>
Interval HeuristicSearchBase<State, Action, StateInfoT>::lookup_bounds(
    StateID state_id) const
{
    return state_infos_[state_id].get_bounds();
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::was_visited(
    StateID state_id) const
{
    return state_infos_[state_id].is_value_initialized();
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::compute_bellman(
    CostFunctionType& cost_function,
    StateID state_id,
    const std::vector<TransitionType>& transitions,
    value_t termination_cost) const -> AlgorithmValueType
{
#if defined(EXPENSIVE_STATISTICS)
    TimerScope scoped_upd_timer(statistics_.update_time);
#endif

    AlgorithmValueType best_value = AlgorithmValueType(termination_cost);

    for (auto& transition : transitions) {
        const value_t cost = cost_function.get_action_cost(transition.action);
        set_min(best_value, compute_qvalue(cost, state_id, transition));
    }

    return best_value;
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::compute_bellman_and_greedy(
    CostFunctionType& cost_function,
    StateID state_id,
    std::vector<TransitionType>& transitions,
    value_t termination_cost,
    std::vector<AlgorithmValueType>& qvalues,
    value_t epsilon) const -> AlgorithmValueType
{
#if defined(EXPENSIVE_STATISTICS)
    TimerScope scoped_upd_timer(statistics_.update_time);
#endif

    if (transitions.empty()) {
        return AlgorithmValueType(termination_cost);
    }

    AlgorithmValueType best_value = compute_q_values(
        cost_function,
        state_id,
        transitions,
        termination_cost,
        qvalues);

    if (as_lower_bound(best_value) == termination_cost) {
        transitions.clear();
        qvalues.clear();
        return AlgorithmValueType(termination_cost);
    }

    filter_greedy_transitions(transitions, qvalues, best_value, epsilon);

    return best_value;
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::select_greedy_transition(
    MDPType& mdp,
    std::optional<Action> previous_greedy,
    std::vector<TransitionType>& transitions) -> std::optional<TransitionType>
{
#if defined(EXPENSIVE_STATISTICS)
    TimerScope scoped(statistics_.policy_selection_time);
#endif

    if (transitions.empty()) return std::nullopt;

    const int index = this->policy_chooser_->pick_index(
        mdp,
        previous_greedy,
        transitions,
        state_infos_);

    assert(utils::in_bounds(index, transitions));

    return std::move(transitions[index]);
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::update_value(
    StateInfo& state_info,
    AlgorithmValueType other,
    value_t epsilon)
{
    ++statistics_.value_updates;
    bool b = algorithms::update(state_info.value, other, epsilon);
    if (b) ++statistics_.value_changes;
    return b;
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::update_policy(
    StateInfo& state_info,
    const std::optional<TransitionType>& transition)
    requires(StorePolicy)
{
    ++statistics_.policy_updates;
    bool b = state_info.update_policy(transition);
    if (b) ++statistics_.policy_changes;
    return b;
}

template <typename State, typename Action, typename StateInfoT>
void HeuristicSearchBase<State, Action, StateInfoT>::initialize_initial_state(
    MDPType& mdp,
    EvaluatorType& h,
    ParamType<State> state)
{
    StateInfo& info = this->state_infos_[mdp.get_state_id(state)];

    if (info.is_value_initialized()) return;

    initialize(mdp, h, state, info);

    statistics_.initial_state_estimate = info.get_value();
    statistics_.initial_state_found_terminal = info.is_goal_or_terminal();
}

template <typename State, typename Action, typename StateInfoT>
void HeuristicSearchBase<State, Action, StateInfoT>::expand_and_initialize(
    MDPType& mdp,
    EvaluatorType& h,
    ParamType<State> state,
    StateInfo& state_info,
    std::vector<TransitionType>& transitions)
{
    assert(!state_info.is_goal_or_terminal());
    assert(transitions.empty());
    assert(state_info.is_on_fringe());

    ++statistics_.expanded_states;
    state_info.removed_from_fringe();

    mdp.generate_all_transitions(state, transitions);

    if (transitions.empty()) {
        ++statistics_.terminal_states;
        state_info.set_terminal();
        return;
    }

    const StateID state_id = mdp.get_state_id(state);

    erase_if(transitions, [&](auto& transition) {
        bool loop = true;
        auto it = transition.successor_dist.begin();
        auto end = transition.successor_dist.end();

        auto loop_it = end;

        for (; it != end; ++it) {
            const auto& [succ_id, prob] = *it;
            if (succ_id == state_id) {
                loop_it = it;
                continue;
            }
            loop = false;
            auto& succ_info = state_infos_[succ_id];
            if (succ_info.is_value_initialized()) continue;
            initialize(mdp, h, mdp.get_state(succ_id), succ_info);
        }

        if (!loop && loop_it != end) {
            value_t prob = loop_it->probability;
            transition.successor_dist.erase(loop_it);
            transition.successor_dist.normalize(1 / (1 - prob));
        }

        return loop;
    });

    if (transitions.empty()) {
        ++statistics_.self_loop_states;
        state_info.set_terminal();
    }
}

template <typename State, typename Action, typename StateInfoT>
void HeuristicSearchBase<State, Action, StateInfoT>::
    generate_non_tip_transitions(
        MDPType& mdp,
        ParamType<State> state,
        std::vector<TransitionType>& transitions) const
{
    assert(transitions.empty());

    mdp.generate_all_transitions(state, transitions);

    const StateID state_id = mdp.get_state_id(state);

    std::erase_if(transitions, [&](auto& transition) {
        bool loop = true;

        for (StateID succ_id : transition.successor_dist.support()) {
            if (succ_id != state_id) loop = false;
        }

        return loop;
    });
}

template <typename State, typename Action, typename StateInfoT>
void HeuristicSearchBase<State, Action, StateInfoT>::print_statistics(
    std::ostream& out) const
{
    out << "  Stored " << sizeof(StateInfo) << " bytes per state" << std::endl;
    statistics_.print(out);
    policy_chooser_->print_statistics(out);
}

template <typename State, typename Action, typename StateInfoT>
void HeuristicSearchBase<State, Action, StateInfoT>::initialize(
    MDPType& mdp,
    EvaluatorType& h,
    ParamType<State> state,
    StateInfo& state_info)
{
    assert(!state_info.is_value_initialized());

    statistics_.evaluated_states++;

    TerminationInfo term = mdp.get_termination_info(state);
    const value_t t_cost = term.get_cost();

    if (term.is_goal_state()) {
        statistics_.goal_states++;
        state_info.set_goal();
        state_info.value = AlgorithmValueType(t_cost);
        return;
    }

    const value_t estimate = h.evaluate(state);

    if constexpr (UseInterval) {
        state_info.value = Interval(estimate, t_cost);
    } else {
        state_info.value = estimate;
    }

    if (estimate == t_cost) {
        statistics_.pruned_states++;
        state_info.set_terminal();
    } else {
        state_info.set_on_fringe();
    }
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::compute_qvalue(
    value_t action_cost,
    StateID state_id,
    const TransitionType& transition) const -> AlgorithmValueType
{
    AlgorithmValueType t_value(action_cost);

    value_t non_loop_prob = 1_vt;

    for (const auto& [succ_id, prob] : transition.successor_dist) {
        if (state_id == succ_id) {
            non_loop_prob -= prob;
            continue;
        }

        t_value += prob * state_infos_[succ_id].value;
    }

    assert(non_loop_prob > 0_vt);

    return t_value * (1_vt / non_loop_prob);
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::compute_q_values(
    CostFunctionType& cost_function,
    StateID state_id,
    std::vector<TransitionType>& transitions,
    value_t termination_cost,
    std::vector<AlgorithmValueType>& qvalues) const -> AlgorithmValueType
{
    AlgorithmValueType best_value(termination_cost);

    qvalues.reserve(transitions.size());

    for (const auto& transition : transitions) {
        const value_t cost = cost_function.get_action_cost(transition.action);
        auto q = compute_qvalue(cost, state_id, transition);
        set_min(best_value, q);
        qvalues.push_back(q);
    }

    return best_value;
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::filter_greedy_transitions(
    std::vector<TransitionType>& transitions,
    std::vector<AlgorithmValueType>& qvalues,
    const AlgorithmValueType& best_value,
    value_t epsilon) const -> AlgorithmValueType
{
    auto view = std::views::zip(transitions, qvalues);
    auto [it, end] = std::ranges::remove_if(
        view,
        [&](const AlgorithmValueType& value) {
            return !is_approx_equal(
                as_lower_bound(best_value),
                as_lower_bound(value),
                epsilon);
        },
        project<1>);

    const size_t offset = std::distance(view.begin(), it);
    transitions.erase(transitions.begin() + offset, transitions.end());

    return best_value;
}

template <typename State, typename Action, typename StateInfoT>
HeuristicSearchAlgorithm<State, Action, StateInfoT>::HeuristicSearchAlgorithm(
    value_t epsilon,
    std::shared_ptr<PolicyPicker> policy_chooser)
    : IterativeMDPAlgorithm<State, Action>(epsilon)
    , HeuristicSearchBase<State, Action, StateInfoT>(std::move(policy_chooser))
{
}

template <typename State, typename Action, typename StateInfoT>
Interval HeuristicSearchAlgorithm<State, Action, StateInfoT>::solve(
    MDPType& mdp,
    EvaluatorType& h,
    ParamType<State> state,
    ProgressReport progress,
    double max_time)
{
    HSBase::initialize_initial_state(mdp, h, state);
    return this->do_solve(mdp, h, state, progress, max_time);
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchAlgorithm<State, Action, StateInfoT>::compute_policy(
    MDPType& mdp,
    EvaluatorType& h,
    ParamType<State> initial_state,
    ProgressReport progress,
    double max_time) -> std::unique_ptr<PolicyType>
{
    this->solve(mdp, h, initial_state, progress, max_time);

    /*
     * Expand some greedy policy graph, starting from the initial state.
     * Collect optimal actions along the way.
     */
    using MapPolicy = policies::MapPolicy<State, Action>;
    std::unique_ptr<MapPolicy> policy(new MapPolicy(&mdp));

    const StateID initial_state_id = mdp.get_state_id(initial_state);

    std::deque<StateID> queue;
    std::set<StateID> visited;
    queue.push_back(initial_state_id);
    visited.insert(initial_state_id);

    std::vector<TransitionType> transitions;
    std::vector<AlgorithmValueType> qvalues;

    do {
        const StateID state_id = queue.front();
        queue.pop_front();

        std::optional<Action> action;

        if constexpr (HSBase::StorePolicy) {
            const StateInfo& state_info = this->state_infos_[state_id];
            action = state_info.get_policy();
        } else {
            const State state = mdp.get_state(state_id);
            const value_t termination_cost =
                mdp.get_termination_info(state).get_cost();

            ClearGuard _(transitions, qvalues);
            this->generate_non_tip_transitions(mdp, state, transitions);

            this->compute_bellman_and_greedy(
                mdp,
                state_id,
                transitions,
                termination_cost,
                qvalues,
                this->epsilon);

            action =
                this->select_greedy_transition(mdp, std::nullopt, transitions)
                    .transform([](const auto& t) { return t.action; });
        }

        // Terminal states have no policy decision.
        if (!action) {
            continue;
        }

        const Interval bound = this->lookup_bounds(state_id);

        policy->emplace_decision(state_id, *action, bound);

        // Push the successor traps.
        const State state = mdp.get_state(state_id);

        Distribution<StateID> successors;
        mdp.generate_action_transitions(state, *action, successors);

        for (const StateID succ_id : successors.support()) {
            if (visited.insert(succ_id).second) {
                queue.push_back(succ_id);
            }
        }
    } while (!queue.empty());

    return policy;
}

template <typename State, typename Action, typename StateInfoT>
void HeuristicSearchAlgorithm<State, Action, StateInfoT>::print_statistics(
    std::ostream& out) const
{
    HSBase::print_statistics(out);
    this->print_additional_statistics(out);
}

} // namespace probfd::algorithms::heuristic_search
