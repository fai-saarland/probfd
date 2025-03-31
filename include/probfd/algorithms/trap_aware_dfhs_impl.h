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
    out << "  Value iterations: " << convergence_value_iterations << std::endl;
    out << "  Bellman backups (forward): " << forward_updates << std::endl;
    out << "  Bellman backups (backward): " << backtracking_updates
        << std::endl;
    out << "  Bellman backups (convergence): " << convergence_updates
        << std::endl;
    out << "  Traps: " << traps << std::endl;
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
bool TADFHSImpl<State, Action, UseInterval>::DFSExplorationState::
    next_successor()
{
    successors.pop_back();
    return !successors.empty();
}

template <typename State, typename Action, bool UseInterval>
StateID
TADFHSImpl<State, Action, UseInterval>::DFSExplorationState::get_successor()
    const
{
    return successors.back();
}

template <typename State, typename Action, bool UseInterval>
TADFHSImpl<State, Action, UseInterval>::TADFHSImpl(
    value_t epsilon,
    std::shared_ptr<QuotientPolicyPicker> policy_chooser,
    bool forward_updates,
    BacktrackingUpdateType backtrack_update_type,
    bool cutoff_tip,
    bool cutoff_inconsistent,
    bool label_solved,
    bool reexpand_traps)
    : Base(policy_chooser)
    , epsilon_(epsilon)
    , forward_updates_(forward_updates)
    , backtrack_update_type_(backtrack_update_type)
    , cutoff_tip_(cutoff_tip)
    , cutoff_inconsistent_(cutoff_inconsistent)
    , label_solved_(label_solved)
    , reexpand_traps_(reexpand_traps)
    , stack_index_(NEW)
{
}

template <typename State, typename Action, bool UseInterval>
Interval TADFHSImpl<State, Action, UseInterval>::solve_quotient(
    QuotientSystem& quotient,
    QHeuristic& heuristic,
    ParamType<QState> qstate,
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
    QHeuristic& heuristic,
    const StateID state,
    ProgressReport& progress,
    utils::CountdownTimer& timer)
{
    bool terminate;
    do {
        terminate = policy_exploration(quotient, heuristic, state, timer) &&
                    value_iteration(quotient, visited_states_, timer);
        visited_states_.clear();
        ++statistics_.iterations;
        progress.print();
    } while (!terminate);
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::dfhs_label_driver(
    QuotientSystem& quotient,
    QHeuristic& heuristic,
    const StateID state,
    ProgressReport& progress,
    utils::CountdownTimer& timer)
{
    bool terminate;
    do {
        terminate = policy_exploration(quotient, heuristic, state, timer);
        ++statistics_.iterations;
        progress.print();
        assert(visited_states_.empty());
    } while (!terminate);
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::advance(
    QuotientSystem& quotient,
    DFSExplorationState& einfo,
    StateInfo& state_info)
{
    using enum BacktrackingUpdateType;

    if (einfo.next_successor()) {
        return true;
    }

    if (backtrack_update_type_ == SINGLE ||
        (backtrack_update_type_ == ON_DEMAND && !einfo.value_converged)) {
        const auto state = quotient.get_state(einfo.state_id);

        ClearGuard _(transitions_, qvalues_);
        this->generate_non_tip_transitions(quotient, state, transitions_);

        ++statistics_.backtracking_updates;

        auto value = this->compute_bellman_and_greedy(
            state,
            transitions_,
            quotient,
            qvalues_);

        auto transition = this->select_greedy_transition(
            quotient,
            state_info.get_policy(),
            transitions_);

        const auto val_upd =
            this->update_value(state_info, value, this->epsilon_);
        bool policy_changed = this->update_policy(state_info, transition);

        // Note: it is only necessary to check whether eps-consistency
        // was reached on backward update when both directions are
        // enabled
        einfo.value_converged = val_upd.converged;

        if (policy_changed) einfo.solved = false;
    }

    if (!einfo.value_converged) einfo.solved = false;

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::push_successor(
    QuotientSystem& quotient,
    DFSExplorationState& einfo,
    StateInfo& sinfo,
    utils::CountdownTimer& timer)
{
    do {
        timer.throw_if_expired();

        const StateID succ = quotient.translate_state_id(einfo.get_successor());
        const uint32_t succ_stack_index = stack_index_[succ];

        if (succ_stack_index == NEW) {
            push(succ);
            return true;
        } else if (succ_stack_index == CLOSED) {
            einfo.is_trap = false;
            if (label_solved_) {
                einfo.solved =
                    einfo.solved && this->state_infos_[succ].is_solved();
            }
        } else {
            // is on stack
            einfo.lowlink = std::min(einfo.lowlink, succ_stack_index);
        }
    } while (advance(quotient, einfo, sinfo));

    return false;
}

template <typename State, typename Action, bool UseInterval>
void TADFHSImpl<State, Action, UseInterval>::push(StateID state_id)
{
    dfs_stack_.emplace_back(state_id, tarjan_stack_.size());
    stack_index_[state_id] = tarjan_stack_.size();
    tarjan_stack_.emplace_back(state_id);
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::initialize(
    QuotientSystem& quotient,
    QHeuristic& heuristic,
    DFSExplorationState& einfo,
    StateInfo& state_info)
{
    if (state_info.is_solved()) {
        assert(label_solved_ || state_info.is_goal_or_terminal());
        einfo.is_trap = false;
        return false;
    }

    const auto state = quotient.get_state(einfo.state_id);

    const bool is_tip_state = state_info.is_on_fringe();

    if (forward_updates_ || is_tip_state) {
        ClearGuard _(transitions_, qvalues_);

        if (is_tip_state) {
            this->expand_and_initialize(
                quotient,
                heuristic,
                state,
                state_info,
                transitions_);
        } else {
            this->generate_non_tip_transitions(quotient, state, transitions_);
        }

        ++statistics_.forward_updates;

        auto value = this->compute_bellman_and_greedy(
            state,
            transitions_,
            quotient,
            qvalues_);

        auto transition = this->select_greedy_transition(
            quotient,
            state_info.get_policy(),
            transitions_);

        const auto val_upd =
            this->update_value(state_info, value, this->epsilon_);
        this->update_policy(state_info, transition);

        einfo.value_converged = val_upd.converged;
        einfo.solved = val_upd.converged;

        if (!transition) {
            einfo.is_trap = false;
            return false;
        }

        const bool cutoff = (cutoff_tip_ && is_tip_state) ||
                            (cutoff_inconsistent_ && !val_upd.converged);

        if (cutoff) {
            einfo.is_trap = false;
            einfo.solved = false;
            return false;
        }

        tarjan_stack_.back().action = transition->action;

        einfo.successors = std::ranges::to<std::vector>(
            successor_dist_.non_source_successor_dist.support());

        einfo.is_trap = quotient.get_action_cost(transition->action) == 0;
    } else {
        auto action = state_info.get_policy();
        if (!action.has_value()) {
            einfo.is_trap = false;
            return false;
        }

        tarjan_stack_.back().action = action;

        einfo.is_trap = quotient.get_action_cost(*action) == 0;

        ClearGuard _(successor_dist_.non_source_successor_dist);
        quotient.generate_action_transitions(state, *action, successor_dist_);

        einfo.successors = std::ranges::to<std::vector>(
            successor_dist_.non_source_successor_dist.support());
    }

    return true;
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::policy_exploration(
    QuotientSystem& quotient,
    QHeuristic& heuristic,
    StateID start_state,
    utils::CountdownTimer& timer)
{
    assert(visited_states_.empty());

    ClearGuard _(stack_index_);

    push(start_state);

    DFSExplorationState* einfo;
    StateInfo* sinfo;

    for (;;) {
        do {
            einfo = &dfs_stack_.back();
            sinfo = &this->state_infos_[einfo->state_id];
        } while (initialize(quotient, heuristic, *einfo, *sinfo) &&
                 push_successor(quotient, *einfo, *sinfo, timer));

        do {
            const uint32_t last_lowlink = einfo->lowlink;
            const bool backtrack_from_scc =
                last_lowlink == stack_index_[einfo->state_id];

            if (backtrack_from_scc) {
                auto scc = tarjan_stack_ | std::views::drop(last_lowlink);

                if (scc.size() > 1 && einfo->is_trap) {
                    ++this->statistics_.traps;

                    const StateID state_id = einfo->state_id;

                    // Collapse trap and reset quotient state data
                    TimerScope scope(statistics_.trap_timer);

                    quotient.build_quotient(scc, *scc.begin());
                    sinfo->update_policy(std::nullopt);
                    sinfo->set_on_fringe();

                    // re-push trap if enabled
                    if (reexpand_traps_) {
                        tarjan_stack_.erase(scc.begin(), scc.end());
                        dfs_stack_.pop_back();
                        push(state_id);
                        break;
                    }

                    stack_index_[state_id] = CLOSED;

                    einfo->value_converged = false;
                    einfo->solved = false;
                } else {
                    for (const StateID state_id :
                         scc | std::views::transform(&StackInfo::state_id)) {
                        stack_index_[state_id] = CLOSED;

                        if (!einfo->solved) continue;

                        StateInfo& mem_info = this->state_infos_[state_id];
                        if (mem_info.is_solved()) continue;

                        if (label_solved_) {
                            mem_info.set_solved();
                        } else {
                            visited_states_.push_back(state_id);
                        }
                    }
                }

                tarjan_stack_.erase(scc.begin(), scc.end());
            }

            DFSExplorationState bt_einfo = std::move(*einfo);
            dfs_stack_.pop_back();

            if (dfs_stack_.empty()) {
                assert(tarjan_stack_.empty());
                return bt_einfo.solved;
            }

            timer.throw_if_expired();

            einfo = &dfs_stack_.back();
            sinfo = &this->state_infos_[einfo->state_id];

            if (backtrack_from_scc) {
                einfo->is_trap = false;
            } else {
                if (!bt_einfo.is_trap) einfo->is_trap = false;
                einfo->lowlink = std::min(last_lowlink, einfo->lowlink);
            }

            if (!bt_einfo.solved) einfo->solved = false;
        } while (!advance(quotient, *einfo, *sinfo) ||
                 !push_successor(quotient, *einfo, *sinfo, timer));
    }
}

template <typename State, typename Action, bool UseInterval>
bool TADFHSImpl<State, Action, UseInterval>::value_iteration(
    QuotientSystem& quotient,
    const std::ranges::input_range auto& range,
    utils::CountdownTimer& timer)
{
    ++statistics_.convergence_value_iterations;

    for (;;) {
        auto [value_changed, policy_changed] = vi_step(quotient, range, timer);

        if (policy_changed) return false;
        if (!value_changed) break;
    }

    return true;
}

template <typename State, typename Action, bool UseInterval>
std::pair<bool, bool> TADFHSImpl<State, Action, UseInterval>::vi_step(
    QuotientSystem& quotient,
    const std::ranges::input_range auto& range,
    utils::CountdownTimer& timer)
{
    bool values_not_conv = false;
    bool policy_not_conv = false;

    for (const StateID id : range) {
        timer.throw_if_expired();

        StateInfo& state_info = this->state_infos_[id];

        const auto state = quotient.get_state(id);

        ClearGuard _(transitions_, qvalues_);

        this->generate_non_tip_transitions(quotient, state, transitions_);

        const auto value = this->compute_bellman_and_greedy(
            state,
            transitions_,
            quotient,
            qvalues_);

        ++statistics_.convergence_updates;

        auto transition = this->select_greedy_transition(
            quotient,
            state_info.get_policy(),
            transitions_);

        auto val_upd = this->update_value(state_info, value, this->epsilon_);
        bool policy_changed = this->update_policy(state_info, transition);
        values_not_conv = values_not_conv || !val_upd.converged;
        policy_not_conv = policy_not_conv || policy_changed;
    }

    return std::make_pair(values_not_conv, policy_not_conv);
}

template <typename State, typename Action, bool UseInterval>
TADepthFirstHeuristicSearch<State, Action, UseInterval>::
    TADepthFirstHeuristicSearch(
        value_t epsilon,
        std::shared_ptr<QuotientPolicyPicker> policy_chooser,
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool label_solved,
        bool reexpand_removed_traps)
    : algorithm_(
          epsilon,
          std::move(policy_chooser),
          forward_updates,
          backtrack_update_type,
          cutoff_tip,
          cutoff_inconsistent,
          label_solved,
          reexpand_removed_traps)
{
}

template <typename State, typename Action, bool UseInterval>
Interval TADepthFirstHeuristicSearch<State, Action, UseInterval>::solve(
    MDPType& mdp,
    HeuristicType& heuristic,
    ParamType<State> state,
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
    HeuristicType& heuristic,
    ParamType<State> state,
    ProgressReport progress,
    double max_time) -> std::unique_ptr<PolicyType>
{
    QuotientSystem quotient(mdp);
    quotients::QuotientMaxHeuristic<State, Action> qheuristic(heuristic);

    auto qinit = quotient.translate_state(state);
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
        const auto quotient_state = quotient.get_state(quotient_id);
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

                SuccessorDistribution successor_dist;
                mdp.generate_action_transitions(source, action, successor_dist);

                for (const StateID succ_id :
                     successor_dist.non_source_successor_dist.support()) {
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
        SuccessorDistribution successor_dist;
        quotient.generate_action_transitions(
            quotient_state,
            *quotient_action,
            successor_dist);

        for (const StateID succ_id :
             successor_dist.non_source_successor_dist.support()) {
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
