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
        auto& info = this->get_state_info(stateid);
        if (info.is_solved()) {
            current_trial_.pop_back();
            break;
        }

        statistics_.trial_bellman_backups++;
        const auto result =
            this->bellman_policy_update(quotient, heuristic, stateid);
        const bool changed = result.value_changed;
        const auto& transition = result.greedy_transition;

        if (!transition) {
            info.set_solved();
            current_trial_.pop_back();
            break;
        }

        if ((stop_at_consistent_ == CONSISTENT && !changed) ||
            (stop_at_consistent_ == INCONSISTENT && changed) ||
            (stop_at_consistent_ == REVISITED && info.is_on_trial())) {
            break;
        }

        if (stop_at_consistent_ == REVISITED) {
            info.set_on_trial();
        }

        current_trial_.push_back(
            this->sample_state(*sample_, stateid, transition->successor_dist));
    }

    statistics_.trial_length += current_trial_.size();
    if (stop_at_consistent_ == REVISITED) {
        for (const StateID state : current_trial_) {
            this->get_state_info(state).clear_trial_flag();
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

    const StateID s = quotient.translate_state_id(this->current_trial_.back());
    auto& sinfo = this->get_state_info(s);

    if (sinfo.is_solved()) {
        // was labeled in some prior check_and_solve() invocation
        return true;
    }

    if (Flags flags; !push(quotient, heuristic, s, flags)) {
        stack_index_.clear();
        return flags.rv;
    }

    ExplorationInformation* einfo = &queue_.back();

    for (;;) {
        while (this->push_successor(quotient, heuristic, *einfo, timer)) {
            einfo = &queue_.back();
        }

        do {
            Flags flags = einfo->flags;

            if (einfo->is_root) {
                const StateID state = einfo->state;
                const unsigned stack_index = stack_index_[state];
                auto scc = stack_ | std::views::drop(stack_index);

                if (scc.size() == 1) {
                    stack_index_[state] = STATE_CLOSED;
                    stack_.pop_back();
                    if (!einfo->flags.rv) {
                        ++this->statistics_.check_and_solve_bellman_backups;
                        this->bellman_policy_update(quotient, heuristic, state);
                    } else {
                        this->get_state_info(state).set_solved();
                    }
                } else {
                    if (einfo->flags.is_trap) {
                        assert(einfo->flags.rv);
                        for (const auto& entry : scc) {
                            stack_index_[entry.state_id] = STATE_CLOSED;
                        }
                        TimerScope scope(statistics_.trap_timer);
                        quotient.build_quotient(scc, *scc.begin());
                        this->get_state_info(state).clear_policy();
                        ++this->statistics_.traps;
                        ++this->statistics_.check_and_solve_bellman_backups;
                        stack_.erase(scc.begin(), scc.end());
                        if (reexpand_traps_) {
                            queue_.pop_back();
                            if (push(quotient, heuristic, state, flags)) {
                                break;
                            } else {
                                goto skip_pop;
                            }
                        } else {
                            this->bellman_policy_update(
                                quotient,
                                heuristic,
                                state);
                            einfo->flags.rv = false;
                        }
                    } else if (einfo->flags.rv) {
                        for (const auto& entry : scc) {
                            stack_index_[entry.state_id] = STATE_CLOSED;
                            this->get_state_info(entry.state_id).set_solved();
                        }
                        stack_.erase(scc.begin(), scc.end());
                    } else {
                        for (const auto& entry : scc) {
                            stack_index_[entry.state_id] = STATE_CLOSED;
                            this->bellman_policy_update(
                                quotient,
                                heuristic,
                                entry.state_id);
                        }
                        stack_.erase(scc.begin(), scc.end());
                    }
                }

                flags.is_trap = false;
            }

            queue_.pop_back();

        skip_pop:;

            if (queue_.empty()) {
                assert(stack_.empty());
                stack_index_.clear();
                return sinfo.is_solved();
            }

            timer.throw_if_expired();

            einfo = &queue_.back();

            einfo->flags.update(flags);
        } while (!einfo->next_successor());
    }
}

template <typename State, typename Action, bool UseInterval>
bool TALRTDPImpl<State, Action, UseInterval>::push_successor(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    ExplorationInformation& einfo,
    utils::CountdownTimer& timer)
{
    do {
        timer.throw_if_expired();

        const StateID succ = quotient.translate_state_id(einfo.get_successor());
        StateInfo& succ_info = this->get_state_info(succ);
        int& sidx = stack_index_[succ];
        if (sidx == STATE_UNSEEN) {
            if (succ_info.is_terminal()) {
                succ_info.set_solved();
            }
            if (succ_info.is_solved()) {
                einfo.flags.update(succ_info);
            } else if (push(quotient, heuristic, succ, einfo.flags)) {
                return true;
            }
            // don't notify_state this state again within this
            // check_and_solve iteration
            sidx = STATE_CLOSED;
        } else if (sidx >= 0) {
            int& sidx2 = stack_index_[einfo.state];
            if (sidx < sidx2) {
                sidx2 = sidx;
                einfo.is_root = false;
            }
        } else {
            einfo.flags.update(succ_info);
        }
    } while (einfo.next_successor());

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TALRTDPImpl<State, Action, UseInterval>::push(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    const StateID state,
    Flags& parent_flags)
{
    assert(quotient.translate_state_id(state) == state);

    ++this->statistics_.check_and_solve_bellman_backups;

    const auto result = this->bellman_policy_update(quotient, heuristic, state);
    const auto& transition = result.greedy_transition;

    if (!transition) {
        assert(this->get_state_info(state).is_dead_end());
        parent_flags.rv = parent_flags.rv && !result.value_changed;
        parent_flags.is_trap = false;
        return false;
    }

    if (result.value_changed) {
        parent_flags.rv = false;
        parent_flags.is_trap = false;
        parent_flags.is_dead = false;
        return false;
    }

    queue_.emplace_back(state);
    ExplorationInformation& e = queue_.back();
    for (const StateID sel : transition->successor_dist.support()) {
        if (sel != state) {
            e.successors.push_back(sel);
        }
    }

    assert(!e.successors.empty());
    e.flags.is_trap = quotient.get_action_cost(transition->action) == 0;
    stack_index_[state] = stack_.size();
    stack_.emplace_back(state, transition->action);
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
void TALRTDP<State, Action, UseInterval>::print_statistics(
    std::ostream& out) const
{
    return algorithm_.print_statistics(out);
}

} // namespace probfd::algorithms::trap_aware_lrtdp
