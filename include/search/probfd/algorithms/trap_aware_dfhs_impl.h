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
TADFHSImpl<State, Action, UseInterval>::TADFHSImpl(
    std::shared_ptr<QuotientPolicyPicker> policy_chooser,
    bool forward_updates,
    BacktrackingUpdateType backtrack_update_type,
    bool expand_tip_states,
    bool cutoff_inconsistent,
    bool stop_exploration_inconsistent,
    bool value_iteration,
    bool mark_solved,
    bool reexpand_removed_traps,
    std::shared_ptr<QuotientOpenList> open_list)
    : Base(policy_chooser)
    , forward_updates_(forward_updates)
    , backtrack_update_type_(backtrack_update_type)
    , expand_tip_states_(expand_tip_states)
    , cutoff_inconsistent_(cutoff_inconsistent)
    , terminate_exploration_(stop_exploration_inconsistent)
    , value_iteration_(value_iteration)
    , mark_solved_(mark_solved)
    , reexpand_removed_traps_(reexpand_removed_traps)
    , open_list_(open_list)
    , stack_index_(STATE_UNSEEN)
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
    Base::initialize_report(quotient, heuristic, qstate, progress);
    statistics_.register_report(progress);

    utils::CountdownTimer timer(max_time);

    StateID state_id = quotient.get_state_id(qstate);
    if (value_iteration_) {
        dfhs_vi_driver(quotient, heuristic, state_id, progress, timer);
    } else {
        dfhs_label_driver(quotient, heuristic, state_id, progress, timer);
    }
    return this->lookup_bounds(state_id);
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
        const bool is_complete =
            policy_exploration(quotient, heuristic, state, timer);
        if (is_complete) {
            vi_res = value_iteration<false>(
                quotient,
                heuristic,
                visited_states_,
                timer);
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
    bool is_complete = false;
    do {
        is_complete = policy_exploration(quotient, heuristic, state, timer) &&
                      this->state_infos_[state].is_solved();
        visited_states_.clear();
        ++statistics_.iterations;
        progress.print();
    } while (!is_complete);
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::enqueue(
    QuotientSystem& quotient,
    StateID state,
    QAction action,
    const Distribution<StateID>& successor_dist)
{
    const bool zero_cost = quotient.get_action_cost(action) == 0;

    queue_.emplace_back(state, stack_.size());
    stack_index_[state] = stack_.size();
    stack_.emplace_back(state, action);

    ExplorationInformation& info = queue_.back();
    info.successors.reserve(successor_dist.size());

    if (open_list_ == nullptr) {
        for (const StateID item : successor_dist.support()) {
            if (item != state) {
                info.successors.push_back(item);
            }
        }
    } else {
        for (const auto& [item, probability] : successor_dist) {
            if (item != state) {
                open_list_->push(state, action, probability, item);
            }
        }
        info.successors.resize(open_list_->size(), StateID::UNDEFINED);
        int i = open_list_->size() - 1;
        while (!open_list_->empty()) {
            info.successors[i] = open_list_->pop();
            --i;
        }
    }

    assert(!info.successors.empty());
    info.flags.is_trap = zero_cost;
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::push_successor(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    ExplorationInformation einfo,
    utils::CountdownTimer& timer)
{
    do {
        timer.throw_if_expired();

        const StateID succ = quotient.translate_state_id(einfo.get_successor());

        int& succ_status = stack_index_[succ];
        if (succ_status == STATE_UNSEEN) {
            // expand state (either not expanded before, or last
            // value change was before pushing einfo.state)
            StateInfo& succ_info = this->state_infos_[succ];
            if (succ_info.is_terminal() || succ_info.is_solved()) {
                succ_info.set_solved();
                einfo.flags.is_trap = false;
            } else if (push_state(
                           quotient,
                           heuristic,
                           succ,
                           succ_info,
                           einfo.flags)) {
                return true;
            } else if (mark_solved_) {
                einfo.flags.all_solved = false;
            }
            succ_status = STATE_CLOSED;
        } else if (succ_status == STATE_CLOSED) {
            const StateInfo& succ_info = this->state_infos_[succ];
            einfo.flags.is_trap = false;
            if (mark_solved_) {
                einfo.flags.all_solved =
                    einfo.flags.all_solved && succ_info.is_solved();
            }
        } else {
            // is on stack
            einfo.lowlink = std::min(einfo.lowlink, succ_status);
        }
    } while (einfo.next_successor() && !terminated_);

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::push_state(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    StateID state_id,
    StateInfo& state_info,
    Flags& flags)
{
    assert(
        !terminated_ && !state_info.is_solved() && !state_info.is_terminal());

    const bool tip = state_info.is_on_fringe();
    if (tip || forward_updates_) {
        ++statistics_.fw_updates;
        auto [value_changed, transition] = this->bellman_policy_update(
            quotient,
            heuristic,
            state_id,
            state_info);
        this->set_policy(state_info, transition);
        flags.all_solved = flags.all_solved && !value_changed;
        const bool cutoff = (!expand_tip_states_ && tip) ||
                            (cutoff_inconsistent_ && value_changed);
        terminated_ = terminate_exploration_ && cutoff;

        if (!transition) {
            flags.is_trap = false;
            return false;
        }

        if (cutoff) {
            flags.complete = false;
            return false;
        }

        enqueue(
            quotient,
            state_id,
            transition->action,
            transition->successor_dist);
    } else {
        const QState state = quotient.get_state(state_id);
        QAction action = *this->get_greedy_action(state_id);
        quotient.generate_action_transitions(state, action, transition_);
        enqueue(quotient, state_id, action, transition_);
        transition_.clear();
    }

    return true;
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::repush_trap(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    StateID state,
    Flags& flags)
{
    flags.clear();
    ++statistics_.fw_updates;

    StateInfo& state_info = this->state_infos_[state];
    auto [value_changed, transition] =
        this->bellman_policy_update(quotient, heuristic, state, state_info);
    this->set_policy(state_info, transition);
    flags.all_solved = !value_changed;
    const bool cutoff =
        !reexpand_removed_traps_ || (cutoff_inconsistent_ && value_changed);
    terminated_ = terminated_ || (terminate_exploration_ && cutoff);

    if (!transition) {
        flags.is_trap = false;
        return false;
    }

    if (cutoff) {
        flags.complete = false;
        return false;
    }

    if (!queue_.empty()) {
        queue_.back().flags.update(flags);
    }

    flags.clear();

    enqueue(quotient, state, transition->action, transition->successor_dist);
    return true;
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::policy_exploration(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    StateID start_state,
    utils::CountdownTimer& timer)
{
    using enum BacktrackingUpdateType;

    assert(visited_states_.empty());
    terminated_ = false;

    {
        Flags flags;
        StateInfo& state_info = this->state_infos_[start_state];
        if (state_info.is_terminal() || state_info.is_solved()) {
            state_info.set_solved();
            flags.is_trap = false;
            return flags.complete;
        }

        if (!push_state(quotient, heuristic, start_state, state_info, flags)) {
            return flags.complete;
        }
    }

    ExplorationInformation* einfo = &queue_.back();
    StateID state = einfo->state;

    for (;;) {
        while (this->push_successor(quotient, heuristic, *einfo, timer)) {
            einfo = &queue_.back();
            state = einfo->state;
        }

        do {
            Flags flags = einfo->flags;
            const int last_lowlink = einfo->lowlink;
            queue_.pop_back();

            flags.complete = flags.complete && !terminated_;

            if (backtrack_update_type_ == SINGLE ||
                (backtrack_update_type_ == ON_DEMAND &&
                 (!flags.complete || !flags.all_solved))) {
                ++statistics_.bw_updates;
                StateInfo& state_info = this->state_infos_[state];
                auto [value_changed, transition] = this->bellman_policy_update(
                    quotient,
                    heuristic,
                    state,
                    state_info);
                bool policy_changed =
                    this->update_policy(state_info, transition);
                flags.complete = flags.complete && !policy_changed;
                flags.all_solved = flags.all_solved && !value_changed;
                terminated_ =
                    terminated_ || (terminate_exploration_ &&
                                    cutoff_inconsistent_ && value_changed);
            }

            // Is SCC root?
            if (einfo->lowlink == stack_index_[state]) {
                auto scc = stack_ | std::views::drop(last_lowlink);

                if (scc.size() == 1) {
                    if (backtrack_update_type_ == CONVERGENCE) {
                        StateInfo& state_info = this->state_infos_[state];
                        auto [value_changed, transition] =
                            this->bellman_policy_update(
                                quotient,
                                heuristic,
                                state,
                                state_info);
                        bool policy_changed =
                            this->update_policy(state_info, transition);
                        flags.complete = flags.complete && !policy_changed;
                        flags.all_solved = flags.all_solved && !value_changed;
                        terminated_ = terminated_ ||
                                      (terminate_exploration_ &&
                                       cutoff_inconsistent_ && value_changed);
                    }
                    backtrack_from_singleton(state, flags);
                } else {
                    if (backtrack_update_type_ == CONVERGENCE) {
                        auto res = value_iteration<true>(
                            quotient,
                            heuristic,
                            scc | std::views::transform(&StackInfo::state_id),
                            timer);
                        flags.complete = flags.complete && !res.policy_changed;
                        flags.all_solved =
                            flags.all_solved && !res.value_changed;
                        terminated_ = terminated_ || (terminate_exploration_ &&
                                                      cutoff_inconsistent_ &&
                                                      res.value_changed);
                    }
                    if (backtrack_from_non_singleton(
                            quotient,
                            heuristic,
                            state,
                            flags,
                            scc)) {
                        break; // re-expanded trap, continue exploring
                    }
                }
            }

            if (queue_.empty()) {
                assert(stack_.empty());
                stack_index_.clear();
                return flags.complete && flags.all_solved;
            }

            timer.throw_if_expired();

            einfo = &queue_.back();
            state = einfo->state;

            einfo->lowlink = std::min(last_lowlink, einfo->lowlink);
            einfo->flags.update(flags);
        } while (!einfo->next_successor() || terminated_);
    }
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::backtrack_from_singleton(
    StateID state,
    Flags& flags)
{
    assert(stack_.back().state_id == state);

    if (flags.complete && flags.all_solved) {
        if (mark_solved_) {
            this->state_infos_[state].set_solved();
        } else if (value_iteration_) {
            visited_states_.push_back(state);
        }
    }

    stack_index_[state] = STATE_CLOSED;
    stack_.pop_back();
    flags.is_trap = false;
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::backtrack_from_non_singleton(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    const StateID state,
    Flags& flags,
    auto scc)
{
    assert(!flags.is_trap || !flags.complete || flags.all_solved);

    if (flags.complete && flags.all_solved) {
        if (flags.is_trap) {
            return backtrack_trap(quotient, heuristic, state, flags, scc);
        }

        backtrack_solved(state, flags, scc);
    } else {
        backtrack_unsolved(state, flags, scc);
    }

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::backtrack_trap(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    StateID state,
    Flags& flags,
    auto scc)
{
    ++this->statistics_.traps;

    for (const auto& entry : scc) {
        stack_index_[entry.state_id] = STATE_CLOSED;
    }

    TimerScope scope(statistics_.trap_timer);

    quotient.build_quotient(scc, *scc.begin());
    this->state_infos_[state].clear_policy();
    stack_.erase(scc.begin(), scc.end());
    return repush_trap(quotient, heuristic, state, flags);
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::backtrack_solved(
    const StateID,
    Flags& flags,
    auto scc)
{
    if (mark_solved_) {
        for (const auto& entry : scc) {
            stack_index_[entry.state_id] = STATE_CLOSED;
            this->state_infos_[entry.state_id].set_solved();
        }
    } else {
        assert(value_iteration_);
        for (const auto& entry : scc) {
            stack_index_[entry.state_id] = STATE_CLOSED;
            visited_states_.push_back(entry.state_id);
        }
    }

    flags.is_trap = false;
    stack_.erase(scc.begin(), scc.end());
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::backtrack_unsolved(
    const StateID,
    Flags& flags,
    auto scc)
{
    for (const auto& entry : scc) {
        stack_index_[entry.state_id] = STATE_CLOSED;
    }

    flags.is_trap = false;
    stack_.erase(scc.begin(), scc.end());
}

template <typename State, typename Action, bool UseInterval>
template <bool Convergence>
auto TADFHSImpl<State, Action, UseInterval>::value_iteration(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
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

            StateInfo& state_info = this->state_infos_[id];
            const auto [value_changed, transition] =
                this->bellman_policy_update(
                    quotient,
                    heuristic,
                    id,
                    state_info);
            bool policy_changed = this->update_policy(state_info, transition);
            value_changed_for_any = value_changed_for_any || value_changed;
            policy_changed_for_any = policy_changed_for_any || policy_changed;
        }

        updated_all.value_changed =
            updated_all.value_changed || value_changed_for_any;
        updated_all.policy_changed =
            updated_all.policy_changed || policy_changed_for_any;
    } while (value_changed_for_any && (Convergence || !policy_changed_for_any));

    return updated_all;
}

template <typename State, typename Action, bool UseInterval>
TADepthFirstHeuristicSearch<State, Action, UseInterval>::
    TADepthFirstHeuristicSearch(
        std::shared_ptr<QuotientPolicyPicker> policy_chooser,
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool expand_tip_states,
        bool cutoff_inconsistent,
        bool stop_exploration_inconsistent,
        bool value_iteration,
        bool mark_solved,
        bool reexpand_removed_traps,
        std::shared_ptr<QuotientOpenList> open_list)
    : algorithm_(
          policy_chooser,
          forward_updates,
          backtrack_update_type,
          expand_tip_states,
          cutoff_inconsistent,
          stop_exploration_inconsistent,
          value_iteration,
          mark_solved,
          reexpand_removed_traps,
          open_list)
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

        std::optional quotient_action =
            algorithm_.get_greedy_action(quotient_id);

        // Terminal states have no policy decision.
        if (!quotient_action) {
            continue;
        }

        const Interval quotient_bound = algorithm_.lookup_bounds(quotient_id);

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
value_t TADepthFirstHeuristicSearch<State, Action, UseInterval>::lookup_value(
    StateID state_id) const
{
    return algorithm_.lookup_value(state_id);
}

template <typename State, typename Action, bool UseInterval>
bool TADepthFirstHeuristicSearch<State, Action, UseInterval>::was_visited(
    StateID state_id) const
{
    return algorithm_.was_visited(state_id);
}

} // namespace probfd::algorithms::trap_aware_dfhs
