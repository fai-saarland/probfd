#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H
#error "This file should only be included from heuristic_search_base.h"
#endif

#include "probfd/algorithms/policy_picker.h"

#include "probfd/algorithms/utils.h"

#include "probfd/policies/map_policy.h"

#include "probfd/utils/language.h"

#include "probfd/evaluator.h"
#include "probfd/mdp.h"
#include "probfd/transition.h"

#include "downward/utils/collections.h"

#include <cassert>
#include <deque>

namespace probfd::algorithms::heuristic_search {

namespace internal {

inline void Statistics::print(std::ostream& out) const
{
    out << "  Stored " << state_info_bytes << " bytes per state" << std::endl;

    out << "  Initial state value estimation: " << initial_state_estimate
        << std::endl;
    out << "  Initial state value found terminal: "
        << initial_state_found_terminal << std::endl;

    out << "  Evaluated state(s): " << evaluated_states << std::endl;
    out << "  Pruned state(s): " << pruned_states << std::endl;
    out << "  Goal state(s): " << goal_states << std::endl;
    out << "  Terminal state(s): " << terminal_states << std::endl;
    out << "  Self-loop state(s): " << self_loop_states << std::endl;
    out << "  Backed up state(s): " << backed_up_states << std::endl;
    out << "  Number of backups: " << backups << std::endl;
    out << "  Number of value changes: " << value_changes << std::endl;
    out << "  Number of policy updates: " << policy_updates << std::endl;

    out << "  Evaluated state(s) until last value change: "
        << before_last_update.evaluated_states << std::endl;
    out << "  Pruned state(s) until last value change: "
        << before_last_update.pruned_states << std::endl;
    out << "  Goal state(s) until last value change: "
        << before_last_update.goal_states << std::endl;
    out << "  Terminal state(s) until last value change: "
        << before_last_update.terminal_states << std::endl;
    out << "  Self-loop state(s) until last value change: "
        << before_last_update.self_loop_states << std::endl;
    out << "  Backed up state(s) until last value change: "
        << before_last_update.backed_up_states << std::endl;
    out << "  Number of backups until last value change: "
        << before_last_update.backups << std::endl;
    out << "  Number of value changes until last value change: "
        << before_last_update.value_changes << std::endl;
    out << "  Number of policy updates until last value change: "
        << before_last_update.policy_updates << std::endl;

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
    statistics_.state_info_bytes = sizeof(StateInfo);
}

template <typename State, typename Action, typename StateInfoT>
value_t HeuristicSearchBase<State, Action, StateInfoT>::lookup_value(
    StateID state_id) const
{
    return state_infos_[state_id].get_value();
}

template <typename State, typename Action, typename StateInfoT>
Interval HeuristicSearchBase<State, Action, StateInfoT>::lookup_bounds(
    StateID state_id) const
{
    return state_infos_[state_id].get_bounds();
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::is_terminal(
    StateID state_id) const
{
    return state_infos_[state_id].is_terminal();
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::is_marked_dead_end(
    StateID state_id) const
{
    return state_infos_[state_id].is_dead_end();
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::was_visited(
    StateID state_id) const
{
    return state_infos_[state_id].is_value_initialized();
}

template <typename State, typename Action, typename StateInfoT>
void HeuristicSearchBase<State, Action, StateInfoT>::clear_policy(
    StateID state_id)
    requires(StorePolicy)
{
    state_infos_[state_id].clear_policy();
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::notify_dead_end(
    StateInfo& state_info,
    value_t termination_cost)
{
    if (!state_info.is_dead_end()) {
        state_info.set_dead_end();
        state_value_changed(state_info);
        state_info.value = AlgorithmValueType(termination_cost);
        return true;
    }

    return false;
}

template <typename State, typename Action, typename StateInfoT>
std::optional<Action>
HeuristicSearchBase<State, Action, StateInfoT>::get_greedy_action(
    StateID state_id)
    requires(StorePolicy)
{
    return state_infos_[state_id].policy;
}

template <typename State, typename Action, typename StateInfoT>
std::optional<Action>
HeuristicSearchBase<State, Action, StateInfoT>::compute_greedy_action(
    MDPType& mdp,
    EvaluatorType& h,
    StateID state_id)
    requires(!StorePolicy)
{
    ClearGuard guard(transitions_);
    const State state = mdp.get_state(state_id);
    mdp.generate_all_transitions(state, transitions_);

    value_t termination_cost = mdp.get_termination_info(state).get_cost();

    filter_greedy_transitions(mdp, h, state_id, transitions_, termination_cost);

    if (transitions_.empty()) return std::nullopt;

    const int index = this->policy_chooser_->pick_index(
        mdp,
        state_id,
        std::nullopt,
        transitions_,
        state_infos_);

    return transitions_[index].action;
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::bellman_update(
    MDPType& mdp,
    EvaluatorType& h,
    StateID state_id)
{
    StateInfo& state_info = this->state_infos_[state_id];
    return bellman_update(mdp, h, state_id, state_info);
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::bellman_update(
    MDPType& mdp,
    EvaluatorType& h,
    StateID state_id,
    std::vector<TransitionType>& greedy)
{
    StateInfo& state_info = this->state_infos_[state_id];
    return bellman_update(mdp, h, state_id, state_info, greedy);
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::bellman_policy_update(
    MDPType& mdp,
    EvaluatorType& h,
    StateID state_id) -> UpdateResult
    requires(StorePolicy)
{
    StateInfo& state_info = this->state_infos_[state_id];

    ClearGuard guard(transitions_);

    const bool value_change =
        bellman_update(mdp, h, state_id, state_info, transitions_);

    if (transitions_.empty()) {
        state_info.clear_policy();
        return UpdateResult{value_change, false};
    }

#if defined(EXPENSIVE_STATISTICS)
    TimerScope scoped(statistics_.policy_selection_time);
#endif

    ++statistics_.policy_updates;

    const int index = this->policy_chooser_->pick_index(
        mdp,
        state_id,
        state_info.get_policy(),
        transitions_,
        state_infos_);
    assert(utils::in_bounds(index, transitions_));

    auto& transition = transitions_[index];

    const bool policy_change = state_info.update_policy(transition.action);

    return UpdateResult{value_change, policy_change, std::move(transition)};
}

template <typename State, typename Action, typename StateInfoT>
void HeuristicSearchBase<State, Action, StateInfoT>::initialize_report(
    MDPType& mdp,
    EvaluatorType& h,
    param_type<State> state,
    ProgressReport& progress)
{
    const StateID initial_id = mdp.get_state_id(state);
    StateInfo& info = this->state_infos_[initial_id];
    initial_state_info_ = &info;

    if (!initialize_if_needed(mdp, h, initial_id, info)) {
        return;
    }

    if constexpr (UseInterval) {
        progress.register_bound("v", [&info]() { return info.value; });
    } else {
        progress.register_bound("v", [&info]() {
            return Interval(info.value, INFINITE_VALUE);
        });
    }

    statistics_.value = info.get_value();
    statistics_.before_last_update =
        static_cast<const internal::CoreStatistics&>(statistics_);
    statistics_.initial_state_estimate = info.get_value();
    statistics_.initial_state_found_terminal = info.is_terminal();
}

template <typename State, typename Action, typename StateInfoT>
void HeuristicSearchBase<State, Action, StateInfoT>::print_statistics(
    std::ostream& out) const
{
    statistics_.print(out);
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::update(
    StateInfo& state_info,
    AlgorithmValueType other)
{
    bool b = algorithms::update(state_info.value, other);
    if (b) state_value_changed(state_info);
    return b;
}

template <typename State, typename Action, typename StateInfoT>
void HeuristicSearchBase<State, Action, StateInfoT>::state_value_changed(
    StateInfo& info)
{
    ++statistics_.value_changes;
    if (&info == initial_state_info_) {
        statistics_.jump();
    }
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::lookup_initialize(
    MDPType& mdp,
    EvaluatorType& h,
    StateID state_id) -> StateInfo&
{
    StateInfo& state_info = this->state_infos_[state_id];
    initialize_if_needed(mdp, h, state_id, state_info);
    return state_info;
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::initialize_if_needed(
    MDPType& mdp,
    EvaluatorType& h,
    StateID state_id,
    StateInfo& state_info)
{
    if (state_info.is_value_initialized()) return false;

    statistics_.evaluated_states++;

    State state = mdp.get_state(state_id);
    TerminationInfo term = mdp.get_termination_info(state);
    const value_t t_cost = term.get_cost();

    if (term.is_goal_state()) {
        state_info.set_goal();
        state_info.value = AlgorithmValueType(t_cost);
        statistics_.goal_states++;
        return true;
    }

    const value_t estimate = h.evaluate(state);
    if (estimate == t_cost) {
        statistics_.pruned_states++;
        notify_dead_end(state_info, t_cost);
    } else {
        state_info.set_on_fringe();

        if constexpr (UseInterval) {
            state_info.value.lower = estimate;
            state_info.value.upper = t_cost;
        } else {
            state_info.value = estimate;
        }
    }

    return true;
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::normalized_qvalue(
    MDPType& mdp,
    EvaluatorType& h,
    StateID state_id,
    const TransitionType& transition) -> std::optional<AlgorithmValueType>
{
    AlgorithmValueType t_value(mdp.get_action_cost(transition.action));
    value_t non_self_loop = 1_vt;
    bool loop = true;

    for (const auto& [succ_id, prob] : transition.successor_dist) {
        if (succ_id == state_id) {
            non_self_loop -= prob;
            continue;
        }

        const auto& succ_info = lookup_initialize(mdp, h, succ_id);
        t_value += prob * succ_info.value;
        loop = false;
    }

    if (loop) return std::nullopt;

    if (non_self_loop < 1_vt) {
        t_value *= (1 / non_self_loop);
    }

    return t_value;
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::filter_greedy_transitions(
    MDPType& mdp,
    EvaluatorType& h,
    StateID state_id,
    std::vector<TransitionType>& transitions,
    value_t termination_cost) -> AlgorithmValueType
{
    using std::ranges::remove_if;

    // First compute the (self-loop normalized) Q values for the lower
    // bound, the minimum Q value, and remove self-loop transitions in the
    // process
    AlgorithmValueType best_value(termination_cost);

    std::vector<value_t> lower_bound_qvalues;
    lower_bound_qvalues.reserve(transitions.size());

    std::erase_if(transitions, [&, state_id](const auto& transition) {
        if (auto q = normalized_qvalue(mdp, h, state_id, transition)) {
            set_min(best_value, *q);
            lower_bound_qvalues.push_back(as_lower_bound(*q));
            return false;
        }

        return true;
    });

    // Now filter non-epsilon-greedy transitions
    if (!transitions.empty()) {
        const value_t best = as_lower_bound(best_value);
        auto view = std::views::zip(transitions, lower_bound_qvalues);
        auto [it, end] = remove_if(view, approx_neq_to(best), project<1>);

        const size_t offset = std::distance(view.begin(), it);
        transitions.erase(transitions.begin() + offset, transitions.end());
    }

    return best_value;
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::bellman_update(
    MDPType& mdp,
    EvaluatorType& h,
    StateID state_id,
    StateInfo& state_info,
    auto&... optional_out_greedy)
{
    static_assert(sizeof...(optional_out_greedy) < 2);

    constexpr bool input_exists = sizeof...(optional_out_greedy) == 1;

    if constexpr (!input_exists) {
        ClearGuard _(transitions_);
    }

#if defined(EXPENSIVE_STATISTICS)
    TimerScope scoped_upd_timer(statistics_.update_time);
#endif
    statistics_.backups++;

    if (state_info.is_terminal()) {
        return false;
    }

    if (state_info.is_on_fringe()) {
        ++statistics_.backed_up_states;
        state_info.removed_from_fringe();
    }

    const State state = mdp.get_state(state_id);

    auto& transitions = [&]() -> auto& {
        if constexpr (input_exists) {
            auto& transitions = select<0>(optional_out_greedy...);
            mdp.generate_all_transitions(state, transitions);
            return transitions;
        } else {
            mdp.generate_all_transitions(state, transitions_);
            return transitions_;
        }
    }();

    const value_t termination_cost = mdp.get_termination_info(state).get_cost();

    if (transitions.empty()) {
        statistics_.terminal_states++;
        return notify_dead_end(state_info, termination_cost);
    }

    AlgorithmValueType best_value;
    bool has_only_self_loops;

    if constexpr (input_exists) {
        best_value = filter_greedy_transitions(
            mdp,
            h,
            state_id,
            transitions,
            termination_cost);
        has_only_self_loops = transitions.empty();
    } else {
        best_value = AlgorithmValueType(termination_cost);
        has_only_self_loops = true;

        for (auto& transition : transitions) {
            if (auto q = normalized_qvalue(mdp, h, state_id, transition)) {
                set_min(best_value, *q);
                has_only_self_loops = false;
            }
        }
    }

    if (has_only_self_loops) {
        statistics_.self_loop_states++;
        return notify_dead_end(state_info, termination_cost);
    }

    return this->update(state_info, best_value);
}

template <typename State, typename Action, typename StateInfoT>
Interval HeuristicSearchAlgorithm<State, Action, StateInfoT>::solve(
    MDPType& mdp,
    EvaluatorType& h,
    param_type<State> state,
    ProgressReport progress,
    double max_time)
{
    HSBase::initialize_report(mdp, h, state, progress);
    this->setup_custom_reports(state, progress);
    return this->do_solve(mdp, h, state, progress, max_time);
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchAlgorithm<State, Action, StateInfoT>::compute_policy(
    MDPType& mdp,
    EvaluatorType& h,
    param_type<State> initial_state,
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

    do {
        const StateID state_id = queue.front();
        queue.pop_front();

        std::optional<Action> action;

        if constexpr (HSBase::StorePolicy) {
            action = this->get_greedy_action(state_id);
        } else {
            action = this->compute_greedy_action(mdp, h, state_id);
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
