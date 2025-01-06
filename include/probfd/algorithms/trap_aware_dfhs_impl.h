#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H
#error "This file should only be included from trap_aware_dfhs.h"
#endif

#include "probfd/algorithms/open_list.h"

#include "probfd/policies/map_policy.h"

#include "probfd/quotients/quotient_max_heuristic.h"

#include "probfd/utils/guards.h"

#include "downward/utils/countdown_timer.h"

#include <cassert>
#include <iterator>
#include <ranges>

namespace probfd::algorithms::trap_aware_dfhs {

namespace internal {

inline void Statistics::print(std::ostream& out) const
{
    out << "  Iterations: " << iterations << std::endl;
    out << "  Traps: " << traps << std::endl;
    out << "  Bellman backups (forward): " << fw_updates << std::endl;
    out << "  Bellman backups (backward): " << bw_updates << std::endl;
    out << "  State re-expansions: " << reexpansions << std::endl;
    out << "  Trap removal time: " << trap_timer << std::endl;
}

inline void Statistics::register_report(ProgressReport& report) const
{
    report.register_print([this](std::ostream& out) {
        out << "iteration=" << iterations << ", traps=" << traps;
    });
}

} // namespace internal

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::ExplorationInformation::
    next_successor()
{
    successors.pop_back();
    return !successors.empty();
}

template <typename State, typename Action, bool UseInterval>
StateID
TADFHSImpl<State, Action, UseInterval>::ExplorationInformation::get_successor()
    const
{
    return successors.back();
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::ExplorationInformation::update(
    const ExplorationInformation& other)
{
    if (!other.value_converged) value_converged = false;
    if (!other.all_solved) all_solved = false;
    if (!other.is_trap) is_trap = false;
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::ExplorationInformation::clear()
{
    value_converged = true;
    all_solved = true;
    is_trap = true;
}

template <typename State, typename Action, bool UseInterval>
TADFHSImpl<State, Action, UseInterval>::TADFHSImpl(
    std::shared_ptr<QuotientPolicyPicker> policy_chooser,
    bool forward_updates,
    BacktrackingUpdateType backtrack_update_type,
    bool cutoff_tip,
    bool cutoff_inconsistent,
    bool terminate_exploration_on_cutoff,
    bool label_solved,
    bool reexpand_traps)
    : Base(policy_chooser)
    , forward_updates_(forward_updates)
    , backtrack_update_type_(backtrack_update_type)
    , cutoff_tip_(cutoff_tip)
    , cutoff_inconsistent_(cutoff_inconsistent)
    , terminate_exploration_on_cutoff_(terminate_exploration_on_cutoff)
    , label_solved_(label_solved)
    , reexpand_traps_(reexpand_traps)
    , stack_index_(NEW)
{
}

template <typename State, typename Action, bool UseInterval>
Interval TADFHSImpl<State, Action, UseInterval>::solve_quotient(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    param_type<QState> qstate,
    ProgressReport& progress,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    Base::initialize_initial_state(quotient, heuristic, qstate);

    const StateID state_id = quotient.get_state_id(qstate);
    const StateInfo& state_info = this->state_infos_[state_id];

    progress.register_bound("v", [&state_info]() {
        return as_interval(state_info.value);
    });

    statistics_.register_report(progress);

    if (!label_solved_) {
        dfhs_vi_driver(quotient, heuristic, state_id, progress, timer);
    } else {
        dfhs_label_driver(quotient, heuristic, state_id, progress, timer);
    }

    return state_info.get_bounds();
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::print_statistics(
    std::ostream& out) const
{
    Base::print_statistics(out);
    statistics_.print(out);
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::dfhs_vi_driver(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    const StateID state,
    ProgressReport& progress,
    utils::CountdownTimer& timer)
{
    UpdateResult vi_res{true, true};
    do {
        const bool solved =
            policy_exploration(quotient, heuristic, state, timer);
        if (solved) {
            vi_res = value_iteration(quotient, visited_states_, timer);
        }
        visited_states_.clear();
        ++statistics_.iterations;
        progress.print();
    } while (vi_res.value_changed || vi_res.policy_changed);
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::dfhs_label_driver(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    const StateID state,
    ProgressReport& progress,
    utils::CountdownTimer& timer)
{
    bool solved;
    do {
        solved = policy_exploration(quotient, heuristic, state, timer) &&
                 this->state_infos_[state].is_solved();
        visited_states_.clear();
        ++statistics_.iterations;
        progress.print();
    } while (!solved);
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::enqueue(
    QuotientSystem& quotient,
    ExplorationInformation& einfo,
    StateID state,
    QAction action,
    const Distribution<StateID>& successor_dist)
{
    stack_.back().action = action;

    einfo.successors.reserve(successor_dist.size());

    for (const StateID item : successor_dist.support()) {
        if (item == state) continue;
        einfo.successors.push_back(item);
    }

    assert(!einfo.successors.empty());
    einfo.is_trap = quotient.get_action_cost(action) == 0;
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::advance(
    QuotientSystem& quotient,
    ExplorationInformation& einfo)
{
    using enum BacktrackingUpdateType;

    if (terminated_) {
        einfo.value_converged = false;
        einfo.all_solved = false;
    } else if (einfo.next_successor()) {
        return true;
    }

    if (backtrack_update_type_ == SINGLE ||
        (backtrack_update_type_ == ON_DEMAND && !einfo.value_converged)) {
        ++statistics_.bw_updates;

        const QState state = quotient.get_state(einfo.state);
        const value_t termination_cost =
            quotient.get_termination_info(state).get_cost();

        ClearGuard _(transitions_, qvalues_);
        this->generate_non_tip_transitions(quotient, state, transitions_);

        StateInfo& state_info = this->state_infos_[einfo.state];
        auto value = this->compute_bellman_and_greedy(
            quotient,
            einfo.state,
            transitions_,
            termination_cost,
            qvalues_);

        bool value_changed = this->update_value(state_info, value);
        bool policy_changed = this->update_policy(
            state_info,
            this->select_greedy_transition(
                quotient,
                state_info.get_policy(),
                transitions_));
        einfo.value_converged = einfo.value_converged && !value_changed;
        einfo.all_solved =
            einfo.all_solved && !value_changed && !policy_changed;
        terminated_ = terminated_ || (terminate_exploration_on_cutoff_ &&
                                      cutoff_inconsistent_ && value_changed);
    }

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::push_successor(
    QuotientSystem& quotient,
    ExplorationInformation& einfo,
    utils::CountdownTimer& timer)
{
    do {
        timer.throw_if_expired();

        const StateID succ = quotient.translate_state_id(einfo.get_successor());

        const int succ_status = stack_index_[succ];

        if (succ_status == NEW) {
            push(succ);
            return true;
        } else if (succ_status == CLOSED) {
            einfo.is_trap = false;
            if (label_solved_) {
                einfo.all_solved =
                    einfo.all_solved && this->state_infos_[succ].is_solved();
            }
        } else {
            // is on stack
            assert(succ_status >= 0);
            einfo.lowlink = std::min(einfo.lowlink, succ_status);
        }
    } while (advance(quotient, einfo));

    return false;
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::push(StateID state_id)
{
    queue_.emplace_back(state_id, stack_.size());
    stack_index_[state_id] = stack_.size();
    stack_.emplace_back(state_id);
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::initialize(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    ExplorationInformation& einfo)
{
    assert(!terminated_);

    const StateID state_id = einfo.state;

    StateInfo& state_info = this->state_infos_[state_id];
    if (state_info.is_solved()) {
        assert(label_solved_ || state_info.is_goal_or_terminal());
        einfo.is_trap = false;
        return false;
    }

    const bool tip = state_info.is_on_fringe();

    if (tip || forward_updates_) {
        ClearGuard _(transitions_, qvalues_);

        const QState state = quotient.get_state(einfo.state);
        const value_t termination_cost =
            quotient.get_termination_info(state).get_cost();

        if (tip) {
            this->expand_and_initialize(
                quotient,
                heuristic,
                state,
                state_info,
                transitions_);
        } else {
            this->generate_non_tip_transitions(quotient, state, transitions_);
        }

        ++statistics_.fw_updates;

        auto value = this->compute_bellman_and_greedy(
            quotient,
            einfo.state,
            transitions_,
            termination_cost,
            qvalues_);

        auto transition = this->select_greedy_transition(
            quotient,
            state_info.get_policy(),
            transitions_);

        bool value_changed = this->update_value(state_info, value);
        this->update_policy(state_info, transition);

        einfo.value_converged = einfo.value_converged && !value_changed;
        einfo.all_solved = einfo.all_solved && !value_changed;
        const bool cutoff =
            (cutoff_tip_ && tip) || (cutoff_inconsistent_ && value_changed);
        terminated_ = terminate_exploration_on_cutoff_ && cutoff;

        if (!transition) {
            einfo.is_trap = false;
            return false;
        }

        if (cutoff) {
            einfo.is_trap = false;
            einfo.value_converged = false;
            einfo.all_solved = false;
            return false;
        }

        enqueue(
            quotient,
            einfo,
            state_id,
            transition->action,
            transition->successor_dist);
    } else {
        auto action = state_info.get_policy();
        if (!action.has_value()) return false;

        const QState state = quotient.get_state(state_id);
        quotient.generate_action_transitions(state, *action, transition_);
        enqueue(quotient, einfo, state_id, *action, transition_);
        transition_.clear();
    }

    return true;
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::policy_exploration(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    StateID start_state,
    utils::CountdownTimer& timer)
{
    assert(visited_states_.empty());
    terminated_ = false;

    push(start_state);

    ExplorationInformation* einfo;

    for (;;) {
        do {
            einfo = &queue_.back();
        } while (initialize(quotient, heuristic, *einfo) &&
                 push_successor(quotient, *einfo, timer));

        do {
            const int last_lowlink = einfo->lowlink;

            // Is SCC root?
            if (einfo->lowlink == stack_index_[einfo->state]) {
                auto scc = stack_ | std::views::drop(last_lowlink);

                if (scc.size() > 1 && einfo->is_trap) {
                    ++this->statistics_.traps;

                    const StateID state_id = einfo->state;

                    // Collapse trap and reset quotient state data
                    TimerScope scope(statistics_.trap_timer);

                    quotient.build_quotient(scc, *scc.begin());
                    StateInfo& state_info = this->state_infos_[state_id];
                    state_info.update_policy(std::nullopt);
                    state_info.set_on_fringe();

                    // re-push trap if enabled
                    if (reexpand_traps_) {
                        stack_.erase(scc.begin(), scc.end());
                        queue_.pop_back();
                        push(state_id);
                        break;
                    }

                    stack_index_[state_id] = CLOSED;

                    einfo->value_converged = false;
                    einfo->all_solved = false;
                } else {
                    for (const auto state_id :
                         scc | std::views::transform(&StackInfo::state_id)) {
                        stack_index_[state_id] = CLOSED;

                        if (!einfo->all_solved) continue;

                        StateInfo& mem_info = this->state_infos_[state_id];
                        if (mem_info.is_solved()) continue;

                        if (label_solved_) {
                            mem_info.set_solved();
                        } else {
                            visited_states_.push_back(state_id);
                        }
                    }
                }

                einfo->is_trap = false;
                stack_.erase(scc.begin(), scc.end());
            }

            ExplorationInformation bt_einfo = std::move(*einfo);
            queue_.pop_back();

            if (queue_.empty()) {
                assert(stack_.empty());
                stack_index_.clear();
                return einfo->all_solved;
            }

            timer.throw_if_expired();

            einfo = &queue_.back();

            einfo->lowlink = std::min(last_lowlink, einfo->lowlink);
            einfo->update(bt_einfo);
        } while (!advance(quotient, *einfo));
    }
}

template <typename State, typename Action, bool UseInterval>
auto TADFHSImpl<State, Action, UseInterval>::value_iteration(
    QuotientSystem& quotient,
    const std::ranges::input_range auto& range,
    utils::CountdownTimer& timer) -> UpdateResult
{
    UpdateResult updated_all(false, false);
    bool value_changed_for_any;
    bool policy_changed_for_any;

    do {
        value_changed_for_any = false;
        policy_changed_for_any = false;

        for (const StateID id : range) {
            timer.throw_if_expired();

            const QState state = quotient.get_state(id);
            const value_t termination_cost =
                quotient.get_termination_info(state).get_cost();

            ClearGuard _(transitions_, qvalues_);
            this->generate_non_tip_transitions(quotient, state, transitions_);

            StateInfo& state_info = this->state_infos_[id];
            const auto value = this->compute_bellman_and_greedy(
                quotient,
                id,
                transitions_,
                termination_cost,
                qvalues_);

            bool value_changed = this->update_value(state_info, value);
            bool policy_changed = this->update_policy(
                state_info,
                this->select_greedy_transition(
                    quotient,
                    state_info.get_policy(),
                    transitions_));
            value_changed_for_any = value_changed_for_any || value_changed;
            policy_changed_for_any = policy_changed_for_any || policy_changed;
        }

        updated_all.value_changed =
            updated_all.value_changed || value_changed_for_any;
        updated_all.policy_changed =
            updated_all.policy_changed || policy_changed_for_any;
    } while (value_changed_for_any && !policy_changed_for_any);

    return updated_all;
}

template <typename State, typename Action, bool UseInterval>
TADepthFirstHeuristicSearch<State, Action, UseInterval>::
    TADepthFirstHeuristicSearch(
        std::shared_ptr<QuotientPolicyPicker> policy_chooser,
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool stop_exploration_inconsistent,
        bool label_solved,
        bool reexpand_removed_traps)
    : algorithm_(
          std::move(policy_chooser),
          forward_updates,
          backtrack_update_type,
          cutoff_tip,
          cutoff_inconsistent,
          stop_exploration_inconsistent,
          label_solved,
          reexpand_removed_traps)
{
}

template <typename State, typename Action, bool UseInterval>
Interval TADepthFirstHeuristicSearch<State, Action, UseInterval>::solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> state,
    ProgressReport progress,
    double max_time)
{
    QuotientSystem quotient(mdp);
    quotients::QuotientMaxHeuristic<State, Action> qheuristic(heuristic);
    return algorithm_.solve_quotient(
        quotient,
        qheuristic,
        quotient.translate_state(state),
        progress,
        max_time);
}

template <typename State, typename Action, bool UseInterval>
auto TADepthFirstHeuristicSearch<State, Action, UseInterval>::compute_policy(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> state,
    ProgressReport progress,
    double max_time) -> std::unique_ptr<PolicyType>
{
    QuotientSystem quotient(mdp);
    quotients::QuotientMaxHeuristic<State, Action> qheuristic(heuristic);

    QState qinit = quotient.translate_state(state);
    algorithm_.solve_quotient(quotient, qheuristic, qinit, progress, max_time);

    /*
     * The quotient policy only specifies the optimal actions between
     * traps. We need to supplement the optimal actions within the
     * traps, i.e. the actions which point every other member state of
     * the trap towards that trap member state that owns the optimal
     * quotient action.
     *
     * We fully explore the quotient policy starting from the initial
     * state and compute the optimal 'inner' actions for each trap. To
     * this end, we first generate the sub-MDP of the trap. Afterwards,
     * we expand the trap graph backwards from the state that has the
     * optimal quotient action. For each encountered state, we select
     * the action with which it is encountered first as the policy
     * action.
     */
    using MapPolicy = policies::MapPolicy<State, Action>;
    std::unique_ptr<MapPolicy> policy(new MapPolicy(&mdp));

    const StateID initial_state_id = quotient.get_state_id(qinit);

    std::deque<StateID> queue({initial_state_id});
    std::set<StateID> visited({initial_state_id});

    do {
        const StateID quotient_id = queue.front();
        const QState quotient_state = quotient.get_state(quotient_id);
        queue.pop_front();

        const auto& state_info = algorithm_.state_infos_[quotient_id];

        std::optional quotient_action = state_info.get_policy();

        // Terminal states have no policy decision.
        if (!quotient_action) {
            continue;
        }

        const Interval quotient_bound = as_interval(state_info.value);

        const StateID exiting_id = quotient_action->state_id;

        policy->emplace_decision(
            exiting_id,
            quotient_action->action,
            quotient_bound);

        // Nothing else needs to be done if the trap has only one state.
        if (quotient_state.num_members() != 1) {
            std::unordered_map<StateID, std::set<QAction>> parents;

            // Build the inverse graph
            std::vector<QAction> inner_actions;
            quotient_state.get_collapsed_actions(inner_actions);

            for (const QAction& qaction : inner_actions) {
                StateID source_id = qaction.state_id;
                Action action = qaction.action;

                const State source = mdp.get_state(source_id);

                Distribution<StateID> successors;
                mdp.generate_action_transitions(source, action, successors);

                for (const StateID succ_id : successors.support()) {
                    parents[succ_id].insert(qaction);
                }
            }

            // Now traverse the inverse graph starting from the exiting
            // state
            std::deque<StateID> inverse_queue({exiting_id});
            std::set<StateID> inverse_visited({exiting_id});

            do {
                const StateID next_id = inverse_queue.front();
                inverse_queue.pop_front();

                for (const auto& [pred_id, act] : parents[next_id]) {
                    if (inverse_visited.insert(pred_id).second) {
                        policy->emplace_decision(pred_id, act, quotient_bound);
                        inverse_queue.push_back(pred_id);
                    }
                }
            } while (!inverse_queue.empty());
        }

        // Push the successor traps.
        Distribution<StateID> successors;
        quotient.generate_action_transitions(
            quotient_state,
            *quotient_action,
            successors);

        for (const StateID succ_id : successors.support()) {
            if (visited.insert(succ_id).second) {
                queue.push_back(succ_id);
            }
        }
    } while (!queue.empty());

    return policy;
}

template <typename State, typename Action, bool UseInterval>
void TADepthFirstHeuristicSearch<State, Action, UseInterval>::print_statistics(
    std::ostream& out) const
{
    return algorithm_.print_statistics(out);
}

template <typename State, typename Action, bool UseInterval>
Interval TADepthFirstHeuristicSearch<State, Action, UseInterval>::lookup_bounds(
    StateID state_id) const
{
    return algorithm_.lookup_bounds(state_id);
}

template <typename State, typename Action, bool UseInterval>
bool TADepthFirstHeuristicSearch<State, Action, UseInterval>::was_visited(
    StateID state_id) const
{
    return algorithm_.was_visited(state_id);
}

} // namespace probfd::algorithms::trap_aware_dfhs
