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
bool TALRTDPImpl<State, Action, UseInterval>::ExplorationInformation::
    next_successor()
{
    successors.pop_back();
    return !successors.empty();
}

template <typename State, typename Action, bool UseInterval>
StateID
TALRTDPImpl<State, Action, UseInterval>::ExplorationInformation::get_successor()
    const
{
    return successors.back();
}

template <typename State, typename Action, bool UseInterval>
TALRTDPImpl<State, Action, UseInterval>::TALRTDPImpl(
    std::shared_ptr<QuotientPolicyPicker> policy_chooser,
    TrialTerminationCondition stop_consistent,
    bool reexpand_traps,
    std::shared_ptr<QuotientSuccessorSampler> succ_sampler)
    : Base(policy_chooser)
    , stop_at_consistent_(stop_consistent)
    , reexpand_traps_(reexpand_traps)
    , sample_(succ_sampler)
    , stack_index_(STATE_UNSEEN)
{
}

template <typename State, typename Action, bool UseInterval>
Interval TALRTDPImpl<State, Action, UseInterval>::solve_quotient(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    param_type<QState> state,
    ProgressReport& progress,
    double max_time)
{
    Base::initialize_report(quotient, heuristic, state, progress);
    this->statistics_.register_report(progress);

    utils::CountdownTimer timer(max_time);

    const StateID state_id = quotient.get_state_id(state);
    bool terminate = false;
    do {
        terminate = trial(quotient, heuristic, state_id, timer);
        statistics_.trials++;
        assert(state_id == quotient.translate_state_id(state_id));
        progress.print();
    } while (!terminate);

    return this->lookup_bounds(state_id);
}

template <typename State, typename Action, bool UseInterval>
void TALRTDPImpl<State, Action, UseInterval>::print_statistics(
    std::ostream& out) const
{
    this->statistics_.print(out);
}

template <typename State, typename Action, bool UseInterval>
bool TALRTDPImpl<State, Action, UseInterval>::trial(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    StateID start_state,
    utils::CountdownTimer& timer)
{
    using enum TrialTerminationCondition;

    assert(current_trial_.empty());

    ClearGuard guard(current_trial_);
    current_trial_.push_back(start_state);
    for (;;) {
        timer.throw_if_expired();

        StateID stateid = current_trial_.back();
        auto& info = this->state_infos_[stateid];
        if (info.is_solved()) {
            current_trial_.pop_back();
            break;
        }

        statistics_.trial_bellman_backups++;
        const auto [value, transition] =
            this->compute_bellman_policy(quotient, heuristic, stateid, info);
        bool value_changed = this->update_value(info, value);
        this->update_policy(info, transition);

        if (!transition) {
            info.set_solved();
            current_trial_.pop_back();
            break;
        }

        if ((stop_at_consistent_ == CONSISTENT && !value_changed) ||
            (stop_at_consistent_ == INCONSISTENT && value_changed) ||
            (stop_at_consistent_ == REVISITED && info.is_on_trial())) {
            break;
        }

        if (stop_at_consistent_ == REVISITED) {
            info.set_on_trial();
        }

        auto next = sample_->sample(
            stateid,
            transition->action,
            transition->successor_dist,
            this->state_infos_);

        current_trial_.push_back(next);
    }

    statistics_.trial_length += current_trial_.size();
    if (stop_at_consistent_ == REVISITED) {
        for (const StateID state : current_trial_) {
            this->state_infos_[state].clear_trial_flag();
        }
    }

    do {
        timer.throw_if_expired();

        if (!check_and_solve(quotient, heuristic, timer)) {
            return false;
        }

        current_trial_.pop_back();
    } while (!current_trial_.empty());

    return true;
}

template <typename State, typename Action, bool UseInterval>
bool TALRTDPImpl<State, Action, UseInterval>::check_and_solve(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    utils::CountdownTimer& timer)
{
    assert(!this->current_trial_.empty());

    push(quotient.translate_state_id(this->current_trial_.back()));

    ExplorationInformation* einfo;
    StateInfo* sinfo;

    for (;;) {
        do {
            einfo = &queue_.back();
            sinfo = &this->state_infos_[einfo->state];
        } while (this->initialize(
                     quotient,
                     heuristic,
                     einfo->state,
                     *sinfo,
                     *einfo) &&
                 this->push_successor(quotient, *einfo, timer));

        do {
            if (einfo->is_root) {
                const StateID state = einfo->state;
                const unsigned stack_index = stack_index_[state];
                auto scc = stack_ | std::views::drop(stack_index);

                if (einfo->is_trap && scc.size() > 1) {
                    assert(einfo->rv);
                    for (const auto& entry : scc) {
                        stack_index_[entry.state_id] = STATE_CLOSED;
                    }
                    TimerScope scope(statistics_.trap_timer);
                    quotient.build_quotient(scc, *scc.begin());
                    sinfo->update_policy(std::nullopt);
                    ++this->statistics_.traps;
                    stack_.erase(scc.begin(), scc.end());
                    if (reexpand_traps_) {
                        queue_.pop_back();
                        push(state);
                        break;
                    }

                    ++this->statistics_.check_and_solve_bellman_backups;
                    auto [value, transition] = this->compute_bellman_policy(
                        quotient,
                        heuristic,
                        state,
                        *sinfo);
                    this->update_value(*sinfo, value);
                    this->update_policy(*sinfo, transition);
                    einfo->rv = false;
                } else {
                    for (const auto& entry : scc) {
                        const StateID id = entry.state_id;
                        StateInfo& info = this->state_infos_[id];
                        stack_index_[id] = STATE_CLOSED;
                        if (info.is_solved()) continue;
                        if (einfo->rv) {
                            info.set_solved();
                        } else {
                            ++this->statistics_.check_and_solve_bellman_backups;
                            auto [value, transition] =
                                this->compute_bellman_policy(
                                    quotient,
                                    heuristic,
                                    id,
                                    info);
                            this->update_value(info, value);
                            this->update_policy(info, transition);
                        }
                    }
                    stack_.erase(scc.begin(), scc.end());
                }

                einfo->is_trap = false;
            }

            ExplorationInformation bt_einfo = std::move(*einfo);

            queue_.pop_back();

            if (queue_.empty()) {
                assert(stack_.empty());
                stack_index_.clear();
                return sinfo->is_solved();
            }

            timer.throw_if_expired();

            einfo = &queue_.back();
            sinfo = &this->state_infos_[einfo->state];

            einfo->update(bt_einfo);
        } while (!einfo->next_successor() ||
                 !this->push_successor(quotient, *einfo, timer));
    }
}

template <typename State, typename Action, bool UseInterval>
bool TALRTDPImpl<State, Action, UseInterval>::push_successor(
    QuotientSystem& quotient,
    ExplorationInformation& einfo,
    utils::CountdownTimer& timer)
{
    do {
        timer.throw_if_expired();

        const StateID succ = quotient.translate_state_id(einfo.get_successor());
        StateInfo& succ_info = this->state_infos_[succ];
        int& sidx = stack_index_[succ];
        if (sidx == STATE_UNSEEN) {
            push(succ);
            return true;
        } else if (sidx >= 0) {
            int& sidx2 = stack_index_[einfo.state];
            if (sidx < sidx2) {
                sidx2 = sidx;
                einfo.is_root = false;
            }
        } else {
            einfo.update(succ_info);
        }
    } while (einfo.next_successor());

    return false;
}

template <typename State, typename Action, bool UseInterval>
void TALRTDPImpl<State, Action, UseInterval>::push(StateID state)
{
    queue_.emplace_back(state);
    stack_index_[state] = stack_.size();
    stack_.emplace_back(state);
}

template <typename State, typename Action, bool UseInterval>
bool TALRTDPImpl<State, Action, UseInterval>::initialize(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    StateID state,
    StateInfo& state_info,
    ExplorationInformation& e_info)
{
    assert(quotient.translate_state_id(state) == state);

    if (state_info.is_solved()) {
        e_info.is_trap = false;
        return false;
    }

    ++this->statistics_.check_and_solve_bellman_backups;

    const auto [value, transition] =
        this->compute_bellman_policy(quotient, heuristic, state, state_info);
    bool value_changed = this->update_value(state_info, value);
    this->update_policy(state_info, transition);

    if (!transition) {
        e_info.rv = e_info.rv && !value_changed;
        e_info.is_trap = false;
        return false;
    }

    if (value_changed) {
        e_info.rv = false;
        e_info.is_trap = false;
        return false;
    }

    for (const StateID sel : transition->successor_dist.support()) {
        if (sel != state) {
            e_info.successors.push_back(sel);
        }
    }

    assert(!e_info.successors.empty());
    e_info.is_trap = quotient.get_action_cost(transition->action) == 0;
    stack_.back().aops.emplace_back(transition->action);
    return true;
}

template <typename State, typename Action, bool UseInterval>
TALRTDP<State, Action, UseInterval>::TALRTDP(
    std::shared_ptr<QuotientPolicyPicker> policy_chooser,
    TrialTerminationCondition stop_consistent,
    bool reexpand_traps,
    std::shared_ptr<QuotientSuccessorSampler> succ_sampler)
    : algorithm_(policy_chooser, stop_consistent, reexpand_traps, succ_sampler)
{
}

template <typename State, typename Action, bool UseInterval>
Interval TALRTDP<State, Action, UseInterval>::solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> s,
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
void TALRTDP<State, Action, UseInterval>::print_statistics(
    std::ostream& out) const
{
    return algorithm_.print_statistics(out);
}

} // namespace probfd::algorithms::trap_aware_lrtdp
