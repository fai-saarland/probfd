#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H
#error "This file should only be included from trap_aware_lrtdp.h"
#endif

#include "probfd/algorithms/successor_sampler.h"

#include "probfd/quotients/quotient_max_heuristic.h"

#include "probfd/utils/guards.h"

#include "downward/utils/countdown_timer.h"

namespace probfd::algorithms::trap_aware_lrtdp {

namespace internal {

inline void Statistics::print(std::ostream& out) const
{
    out << "  Trials: " << trials << std::endl;
    out << "  Average trial length: "
        << (static_cast<double>(trial_length) / static_cast<double>(trials))
        << std::endl;
    out << "  Bellman backups (trials): " << trial_bellman_backups << std::endl;
    out << "  Bellman backups (check&solved): "
        << check_and_solve_bellman_backups << std::endl;
    out << "  Trap removals: " << traps << std::endl;
    out << "  Trap removal time: " << trap_timer << std::endl;
}

inline void Statistics::register_report(ProgressReport& report) const
{
    report.register_print([this](std::ostream& out) {
        out << "traps=" << traps << ", trials=" << trials;
    });
}

} // namespace internal

template <typename State, typename Action, bool UseInterval>
bool TALRTDPImpl<State, Action, UseInterval>::DFSExplorationState::
    next_successor()
{
    successors.pop_back();
    return !successors.empty();
}

template <typename State, typename Action, bool UseInterval>
StateID
TALRTDPImpl<State, Action, UseInterval>::DFSExplorationState::get_successor()
    const
{
    return successors.back();
}

template <typename State, typename Action, bool UseInterval>
TALRTDPImpl<State, Action, UseInterval>::TALRTDPImpl(
    value_t epsilon,
    std::shared_ptr<QuotientPolicyPicker> policy_chooser,
    TrialTerminationCondition stop_consistent,
    bool reexpand_traps,
    std::shared_ptr<QuotientSuccessorSampler> succ_sampler)
    : Base(policy_chooser)
    , epsilon_(epsilon)
    , stop_at_consistent_(stop_consistent)
    , reexpand_traps_(reexpand_traps)
    , sample_(succ_sampler)
    , stack_index_(STATE_UNSEEN)
{
}

template <typename State, typename Action, bool UseInterval>
Interval TALRTDPImpl<State, Action, UseInterval>::solve_quotient(
    QuotientSystem& quotient,
    QHeuristic& heuristic,
    ParamType<QState> state,
    ProgressReport& progress,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    Base::initialize_initial_state(quotient, heuristic, state);

    const StateID state_id = quotient.get_state_id(state);
    const StateInfo& state_info = this->state_infos_[state_id];

    progress.register_bound("v", [&state_info]() {
        return as_interval(state_info.value);
    });

    this->statistics_.register_report(progress);

    bool terminate;
    do {
        terminate = trial(quotient, heuristic, state_id, timer);
        assert(state_id == quotient.translate_state_id(state_id));
        statistics_.trials++;
        progress.print();
    } while (!terminate);

    return state_info.get_bounds();
}

template <typename State, typename Action, bool UseInterval>
void TALRTDPImpl<State, Action, UseInterval>::print_statistics(
    std::ostream& out) const
{
    this->statistics_.print(out);
    this->sample_->print_statistics(out);
}

template <typename State, typename Action, bool UseInterval>
bool TALRTDPImpl<State, Action, UseInterval>::trial(
    QuotientSystem& quotient,
    QHeuristic& heuristic,
    StateID start_state,
    utils::CountdownTimer& timer)
{
    using enum TrialTerminationCondition;

    assert(current_trial_.empty());

    ClearGuard guard(current_trial_);
    current_trial_.push_back(start_state);

    for (;;) {
        timer.throw_if_expired();

        const StateID state_id = current_trial_.back();
        auto& state_info = this->state_infos_[state_id];

        if (state_info.is_solved()) {
            current_trial_.pop_back();
            break;
        }

        const QState state = quotient.get_state(state_id);

        ClearGuard _(transitions_, qvalues_);

        if (state_info.is_on_fringe()) {
            this->expand_and_initialize(
                quotient,
                heuristic,
                state,
                state_info,
                transitions_);
        } else {
            this->generate_non_tip_transitions(quotient, state, transitions_);
        }

        statistics_.trial_bellman_backups++;

        const auto value = this->compute_bellman_and_greedy(
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

        if (!transition.has_value()) {
            state_info.set_solved();
            current_trial_.pop_back();
            break;
        }

        assert(!state_info.is_goal_or_terminal());

        if ((stop_at_consistent_ == CONSISTENT && val_upd.converged) ||
            (stop_at_consistent_ == INCONSISTENT && !val_upd.converged) ||
            (stop_at_consistent_ == REVISITED && state_info.is_on_trial())) {
            break;
        }

        if (stop_at_consistent_ == REVISITED) {
            state_info.set_on_trial();
        }

        assert(!transition->successor_dist.non_source_successor_dist.empty());

        auto next = sample_->sample(
            state_id,
            transition->action,
            transition->successor_dist,
            this->state_infos_);

        current_trial_.push_back(next);
    }

    using std::views::reverse, std::views::drop;

    statistics_.trial_length += current_trial_.size();

    if (stop_at_consistent_ == REVISITED) {
        for (const StateID state : current_trial_ | reverse | drop(1)) {
            auto& info = this->state_infos_[state];
            assert(info.is_on_trial());
            info.clear_trial_flag();
        }
    }

    do {
        timer.throw_if_expired();

        if (!check_and_solve(
                quotient,
                heuristic,
                current_trial_.back(),
                timer)) {
            return false;
        }

        current_trial_.pop_back();
    } while (!current_trial_.empty());

    return true;
}

template <typename State, typename Action, bool UseInterval>
bool TALRTDPImpl<State, Action, UseInterval>::check_and_solve(
    QuotientSystem& quotient,
    QHeuristic& heuristic,
    StateID init_state_id,
    utils::CountdownTimer& timer)
{
    assert(!this->current_trial_.empty());

    ClearGuard _(stack_index_);

    push(quotient.translate_state_id(init_state_id));

    DFSExplorationState* einfo;
    StateInfo* sinfo;

    for (;;) {
        do {
            einfo = &dfs_stack_.back();
            sinfo = &this->state_infos_[einfo->state];
        } while (this->initialize(
                     quotient,
                     heuristic,
                     einfo->state,
                     *sinfo,
                     *einfo) &&
                 this->push_successor(quotient, *einfo, timer));

        do {
            const uint32_t last_lowlink = einfo->lowlink;
            const bool is_scc_root = last_lowlink == stack_index_[einfo->state];

            if (is_scc_root) {
                auto scc = tarjan_stack_ | std::views::drop(last_lowlink);

                // Mark all states as closed
                for (const auto& entry : scc) {
                    stack_index_[entry.state_id] = STATE_CLOSED;
                }

                if (einfo->is_trap && scc.size() > 1) {
                    ++statistics_.traps;

                    TimerScope scope(statistics_.trap_timer);
                    quotient.build_quotient(scc, *scc.begin());
                    sinfo->update_policy(std::nullopt);

                    if (reexpand_traps_) {
                        tarjan_stack_.erase(scc.begin(), scc.end());
                        dfs_stack_.pop_back();
                        push(einfo->state);
                        break;
                    }

                    const QState state = quotient.get_state(einfo->state);
                    do_non_tip_bellman_update(quotient, state, *sinfo);

                    einfo->rv = false;
                } else if (einfo->rv) {
                    for (const auto& entry : scc) {
                        const StateID id = entry.state_id;
                        StateInfo& info = this->state_infos_[id];
                        info.set_solved();
                    }
                } else {
                    for (const auto& entry : scc) {
                        const StateID id = entry.state_id;
                        StateInfo& info = this->state_infos_[id];
                        if (info.is_solved()) continue;

                        const QState state = quotient.get_state(id);
                        do_non_tip_bellman_update(quotient, state, info);
                    }
                }

                tarjan_stack_.erase(scc.begin(), scc.end());
            }

            DFSExplorationState bt_einfo = std::move(*einfo);

            dfs_stack_.pop_back();

            if (dfs_stack_.empty()) {
                assert(tarjan_stack_.empty());
                return sinfo->is_solved();
            }

            timer.throw_if_expired();

            einfo = &dfs_stack_.back();
            sinfo = &this->state_infos_[einfo->state];

            einfo->lowlink = std::min(last_lowlink, einfo->lowlink);

            if (is_scc_root) einfo->is_trap = false;
            if (!bt_einfo.rv) einfo->rv = false;
        } while (!einfo->next_successor() ||
                 !this->push_successor(quotient, *einfo, timer));
    }
}

template <typename State, typename Action, bool UseInterval>
bool TALRTDPImpl<State, Action, UseInterval>::push_successor(
    QuotientSystem& quotient,
    DFSExplorationState& einfo,
    utils::CountdownTimer& timer)
{
    do {
        timer.throw_if_expired();

        const StateID succ = quotient.translate_state_id(einfo.get_successor());
        uint32_t sidx = stack_index_[succ];

        if (sidx == STATE_UNSEEN) {
            push(succ);
            return true;
        } else if (sidx == STATE_CLOSED) {
            einfo.is_trap = false;
            einfo.rv = einfo.rv && this->state_infos_[succ].is_solved();
        } else {
            einfo.lowlink = std::min(einfo.lowlink, sidx);
        }
    } while (einfo.next_successor());

    return false;
}

template <typename State, typename Action, bool UseInterval>
void TALRTDPImpl<State, Action, UseInterval>::push(StateID state)
{
    dfs_stack_.emplace_back(state, tarjan_stack_.size());
    stack_index_[state] = tarjan_stack_.size();
    tarjan_stack_.emplace_back(state);
}

template <typename State, typename Action, bool UseInterval>
bool TALRTDPImpl<State, Action, UseInterval>::initialize(
    QuotientSystem& quotient,
    QHeuristic& heuristic,
    StateID state_id,
    StateInfo& state_info,
    DFSExplorationState& e_info)
{
    assert(quotient.translate_state_id(state_id) == state_id);

    if (state_info.is_solved()) {
        if constexpr (UseInterval) {
            assert(state_info.value.bounds_approximately_equal(this->epsilon_));
        }

        e_info.rv = true;
        return false;
    }

    const QState state = quotient.get_state(state_id);

    ClearGuard _(transitions_, qvalues_);

    if (state_info.is_on_fringe()) {
        this->expand_and_initialize(
            quotient,
            heuristic,
            state,
            state_info,
            transitions_);
    } else {
        this->generate_non_tip_transitions(quotient, state, transitions_);
    }

    ++this->statistics_.check_and_solve_bellman_backups;

    const auto value = this->compute_bellman_and_greedy(
        state,
        transitions_,
        quotient,
        qvalues_);

    auto transition = this->select_greedy_transition(
        quotient,
        state_info.get_policy(),
        transitions_);

    const auto val_upd = this->update_value(state_info, value, this->epsilon_);
    this->update_policy(state_info, transition);

    if (!transition.has_value()) {
        assert(val_upd.converged);
        e_info.rv = true;
        return false;
    }

    // cut off if value has changed by more than epsilon
    if (!val_upd.changed) {
        e_info.rv = val_upd.converged;

        for (const StateID sel :
             transition->successor_dist.non_source_successor_dist.support()) {
            e_info.successors.push_back(sel);
        }

        assert(!e_info.successors.empty());
        e_info.is_trap = quotient.get_action_cost(transition->action) == 0;
        tarjan_stack_.back().aops.emplace_back(transition->action);
        return true;
    }

    // not solved if cut off because successors not generated
    e_info.rv = false;
    return false;
}

template <typename State, typename Action, bool UseInterval>
void TALRTDPImpl<State, Action, UseInterval>::do_non_tip_bellman_update(
    QuotientSystem& quotient,
    const QState& state,
    StateInfo& info)
{
    ClearGuard _(transitions_, qvalues_);
    this->generate_non_tip_transitions(quotient, state, transitions_);

    ++this->statistics_.check_and_solve_bellman_backups;

    auto value = this->compute_bellman_and_greedy(
        state,
        transitions_,
        quotient,
        qvalues_);

    auto transition = this->select_greedy_transition(
        quotient,
        info.get_policy(),
        transitions_);

    this->update_value(info, value, this->epsilon_);
    this->update_policy(info, transition);
}

template <typename State, typename Action, bool UseInterval>
TALRTDP<State, Action, UseInterval>::TALRTDP(
    value_t epsilon,
    std::shared_ptr<QuotientPolicyPicker> policy_chooser,
    TrialTerminationCondition stop_consistent,
    bool reexpand_traps,
    std::shared_ptr<QuotientSuccessorSampler> succ_sampler)
    : algorithm_(
          epsilon,
          policy_chooser,
          stop_consistent,
          reexpand_traps,
          succ_sampler)
{
}

template <typename State, typename Action, bool UseInterval>
Interval TALRTDP<State, Action, UseInterval>::solve(
    MDPType& mdp,
    HeuristicType& heuristic,
    ParamType<State> s,
    ProgressReport progress,
    double max_time)
{
    QuotientSystem quotient(mdp);
    quotients::QuotientMaxHeuristic<State, Action> qheuristic(heuristic);
    return algorithm_.solve_quotient(
        quotient,
        qheuristic,
        quotient.translate_state(s),
        progress,
        max_time);
}

template <typename State, typename Action, bool UseInterval>
auto TALRTDP<State, Action, UseInterval>::compute_policy(
    MDPType& mdp,
    HeuristicType& heuristic,
    ParamType<State> state,
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
void TALRTDP<State, Action, UseInterval>::print_statistics(
    std::ostream& out) const
{
    return algorithm_.print_statistics(out);
}

} // namespace probfd::algorithms::trap_aware_lrtdp
