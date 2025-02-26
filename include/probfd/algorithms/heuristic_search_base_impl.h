#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H
#error "This file should only be included from heuristic_search_base.h"
#endif

#include "probfd/algorithms/policy_picker.h"

#include "probfd/algorithms/utils.h"

#include "probfd/policies/map_policy.h"

#include "probfd/utils/language.h"

#include "probfd/heuristic.h"
#include "probfd/mdp.h"
#include "probfd/transition_tail.h"

#include "downward/utils/collections.h"

#include <cassert>
#include <deque>

namespace probfd::algorithms::heuristic_search {

namespace internal {

inline void Statistics::print(std::ostream& out) const
{
    out << "  Initial state value estimation: " << initial_state_estimate
        << std::endl;
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
    ParamType<State> source_state,
    const std::vector<TransitionTailType>& transition_tails,
    CostFunctionType& cost_function) const -> AlgorithmValueType
{
#if defined(EXPENSIVE_STATISTICS)
    TimerScope scoped_upd_timer(statistics_.update_time);
#endif

    AlgorithmValueType best_value(
        cost_function.get_termination_cost(source_state));

    for (auto& transition_tail : transition_tails) {
        set_min(best_value, compute_qvalue(transition_tail, cost_function));
    }

    return best_value;
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::compute_bellman_and_greedy(
    ParamType<State> source_state,
    std::vector<TransitionTailType>& transition_tails,
    CostFunctionType& cost_function,
    std::vector<AlgorithmValueType>& qvalues) const -> AlgorithmValueType
{
#if defined(EXPENSIVE_STATISTICS)
    TimerScope scoped_upd_timer(statistics_.update_time);
#endif

    if (transition_tails.empty()) {
        return AlgorithmValueType(
            cost_function.get_termination_cost(source_state));
    }

    AlgorithmValueType best_value =
        compute_q_values(transition_tails, cost_function, qvalues);

    const value_t termination_cost =
        cost_function.get_termination_cost(source_state);

    if (as_lower_bound(best_value) >= termination_cost) {
        transition_tails.clear();
        qvalues.clear();
        return AlgorithmValueType(termination_cost);
    }

    filter_greedy_transitions(transition_tails, qvalues, best_value);

    return best_value;
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::select_greedy_transition(
    MDPType& mdp,
    std::optional<Action> previous_greedy,
    std::vector<TransitionTailType>& transition_tails)
    -> std::optional<TransitionTailType>
{
#if defined(EXPENSIVE_STATISTICS)
    TimerScope scoped(statistics_.policy_selection_time);
#endif

    if (transition_tails.empty()) return std::nullopt;

    const int index = this->policy_chooser_->pick_index(
        mdp,
        previous_greedy,
        transition_tails,
        state_infos_);

    assert(utils::in_bounds(index, transition_tails));

    return std::move(transition_tails[index]);
}

template <typename State, typename Action, typename StateInfoT>
ValueUpdateResult HeuristicSearchBase<State, Action, StateInfoT>::update_value(
    StateInfo& state_info,
    AlgorithmValueType other,
    value_t epsilon)
{
    ++statistics_.value_updates;
    auto r = algorithms::update(state_info.value, other, epsilon);
    if (r.changed) ++statistics_.value_changes;
    return r;
}

template <typename State, typename Action, typename StateInfoT>
bool HeuristicSearchBase<State, Action, StateInfoT>::update_policy(
    StateInfo& state_info,
    const std::optional<TransitionTailType>& transition)
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
    HeuristicType& h,
    ParamType<State> state)
{
    StateInfo& info = this->state_infos_[mdp.get_state_id(state)];

    if (info.is_value_initialized()) return;

    initialize(mdp, h, state, info);

    statistics_.initial_state_estimate = info.get_value();
}

template <typename State, typename Action, typename StateInfoT>
void HeuristicSearchBase<State, Action, StateInfoT>::expand_and_initialize(
    MDPType& mdp,
    HeuristicType& h,
    ParamType<State> state,
    StateInfo& state_info,
    std::vector<TransitionTailType>& transition_tails)
{
    assert(!state_info.is_goal_or_terminal());
    assert(transition_tails.empty());
    assert(state_info.is_on_fringe());

    ++statistics_.expanded_states;
    state_info.removed_from_fringe();

    mdp.generate_all_transitions(state, transition_tails);

    if (transition_tails.empty()) {
        ++statistics_.terminal_states;
        state_info.set_terminal();
        return;
    }

    std::erase_if(transition_tails, [&](auto& transition) {
        return transition.successor_dist.non_source_successor_dist.empty();
    });

    if (transition_tails.empty()) {
        ++statistics_.self_loop_states;
        state_info.set_terminal();
        return;
    }

    for (auto& transition : transition_tails) {
        for (const auto& [succ_id, prob] :
             transition.successor_dist.non_source_successor_dist) {
            auto& succ_info = state_infos_[succ_id];
            if (succ_info.is_value_initialized()) continue;
            initialize(mdp, h, mdp.get_state(succ_id), succ_info);
        }
    }
}

template <typename State, typename Action, typename StateInfoT>
void HeuristicSearchBase<State, Action, StateInfoT>::
    generate_non_tip_transitions(
        MDPType& mdp,
        ParamType<State> state,
        std::vector<TransitionTailType>& transition_tails) const
{
    assert(transition_tails.empty());

    mdp.generate_all_transitions(state, transition_tails);

    std::erase_if(transition_tails, [&](auto& transition) {
        return transition.successor_dist.non_source_successor_dist.empty();
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
    HeuristicType& h,
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
    const TransitionTailType& transition,
    CostFunctionType& cost_function) const -> AlgorithmValueType
{
    AlgorithmValueType t_value(
        cost_function.get_action_cost(transition.action));

    for (const auto& [succ_id, prob] :
         transition.successor_dist.non_source_successor_dist) {
        t_value += prob * state_infos_[succ_id].value;
    }

    assert(transition.successor_dist.non_source_probability != 0_vt);

    return t_value / transition.successor_dist.non_source_probability;
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::compute_q_values(
    std::vector<TransitionTailType>& transition_tails,
    CostFunctionType& cost_function,
    std::vector<AlgorithmValueType>& qvalues) const -> AlgorithmValueType
{
    AlgorithmValueType best_value(INFINITE_VALUE);

    qvalues.reserve(transition_tails.size());

    for (const auto& transition : transition_tails) {
        auto q = compute_qvalue(transition, cost_function);
        set_min(best_value, q);
        qvalues.push_back(q);
    }

    return best_value;
}

template <typename State, typename Action, typename StateInfoT>
auto HeuristicSearchBase<State, Action, StateInfoT>::filter_greedy_transitions(
    std::vector<TransitionTailType>& transition_tails,
    std::vector<AlgorithmValueType>& qvalues,
    const AlgorithmValueType& best_value) const -> AlgorithmValueType
{
    auto view = std::views::zip(transition_tails, qvalues);
    auto [it, end] = std::ranges::remove_if(
        view,
        [&](const AlgorithmValueType& value) {
            return as_lower_bound(best_value) != as_lower_bound(value);
        },
        project<1>);

    const size_t offset = std::distance(view.begin(), it);
    transition_tails.erase(
        transition_tails.begin() + offset,
        transition_tails.end());

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
    HeuristicType& h,
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
    HeuristicType& h,
    ParamType<State> initial_state,
    ProgressReport progress,
    double max_time) -> std::unique_ptr<PolicyType>
{
    this->solve(mdp, h, initial_state, progress, max_time);

    /*
     * Expand some greedy policy graph, starting from the initial state.
     * Collect optimal actions along the way.
     */
    auto policy = std::make_unique<policies::MapPolicy<State, Action>>(&mdp);

    const StateID initial_state_id = mdp.get_state_id(initial_state);

    std::deque<StateID> queue;
    std::set<StateID> visited;
    queue.push_back(initial_state_id);
    visited.insert(initial_state_id);

    std::vector<TransitionTailType> transition_tails;
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

            ClearGuard _(transition_tails, qvalues);
            this->generate_non_tip_transitions(mdp, state, transition_tails);

            this->compute_bellman_and_greedy(
                state,
                transition_tails,
                mdp,
                qvalues);

            action = this->select_greedy_transition(
                             mdp,
                             std::nullopt,
                             transition_tails)
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

        SuccessorDistribution successor_dist;
        mdp.generate_action_transitions(state, *action, successor_dist);

        for (const StateID succ_id :
             successor_dist.non_source_successor_dist.support()) {
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
