#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_LRTDP_H
#error "This file should only be included from lrtdp.h"
#endif

#include "probfd/algorithms/successor_sampler.h"

#include "downward/utils/countdown_timer.h"

#include <cassert>
#include <ranges>

namespace probfd::algorithms::lrtdp {

namespace internal {

inline void Statistics::print(std::ostream& out) const
{
    out << "  Trials: " << trials << std::endl;
    out << "  Bellman backups (trials): " << trial_bellman_backups << std::endl;
    out << "  Bellman backups (check&solved): "
        << check_and_solve_bellman_backups << std::endl;
}

} // namespace internal

template <typename State, typename Action, bool UseInterval>
LRTDP<State, Action, UseInterval>::LRTDP(
    std::shared_ptr<PolicyPickerType> policy_chooser,
    TrialTerminationCondition stop_consistent,
    std::shared_ptr<SuccessorSamplerType> succ_sampler)
    : Base(policy_chooser)
    , stop_consistent_(stop_consistent)
    , sample_(succ_sampler)
{
}

template <typename State, typename Action, bool UseInterval>
void LRTDP<State, Action, UseInterval>::reset_search_state()
{
    this->state_infos_.reset();
}

template <typename State, typename Action, bool UseInterval>
Interval LRTDP<State, Action, UseInterval>::do_solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> state,
    ProgressReport& progress,
    double max_time)
{
    progress.register_print(
        [&](std::ostream& out) { out << "trials=" << statistics_.trials; });

    utils::CountdownTimer timer(max_time);

    const StateID state_id = mdp.get_state_id(state);

    for (;;) {
        StateInfo& info = this->state_infos_[state_id];

        if (info.is_solved()) {
            break;
        }

        trial(mdp, heuristic, state_id, timer);
        this->statistics_.trials++;
        progress.print();
    }

    return this->state_infos_[state_id].get_bounds();
}

template <typename State, typename Action, bool UseInterval>
void LRTDP<State, Action, UseInterval>::print_additional_statistics(
    std::ostream& out) const
{
    statistics_.print(out);
}

template <typename State, typename Action, bool UseInterval>
void LRTDP<State, Action, UseInterval>::trial(
    MDPType& mdp,
    EvaluatorType& heuristic,
    StateID initial_state,
    utils::CountdownTimer& timer)
{
    assert(!this->state_infos_[initial_state].is_solved());

    using enum TrialTerminationCondition;

    ClearGuard guard(current_trial_);

    current_trial_.push_back(initial_state);

    for (;;) {
        timer.throw_if_expired();

        const StateID state_id = current_trial_.back();

        auto& state_info = this->state_infos_[state_id];

        if (state_info.is_solved()) {
            current_trial_.pop_back();
            break;
        }

        this->statistics_.trial_bellman_backups++;

        auto [value, transition] =
            this->compute_bellman_policy(mdp, heuristic, state_id, state_info);

        bool value_changed = this->update_value(state_info, value);
        this->update_policy(state_info, transition);

        if (!transition) {
            // terminal
            assert(state_info.is_terminal());
            state_info.mark_solved();
            current_trial_.pop_back();
            break;
        }

        assert(!state_info.is_terminal());

        if ((stop_consistent_ == CONSISTENT && !value_changed) ||
            (stop_consistent_ == INCONSISTENT && value_changed) ||
            (stop_consistent_ == REVISITED && state_info.is_closed())) {
            break;
        }

        if (stop_consistent_ == REVISITED) {
            state_info.mark_closed();
        }

        auto next = sample_->sample(
            state_id,
            transition->action,
            transition->successor_dist,
            this->state_infos_);

        current_trial_.push_back(next);
    }

    if (stop_consistent_ == REVISITED) {
        for (const StateID state :
             current_trial_ | std::views::reverse | std::views::drop(1)) {
            auto& info = this->state_infos_[state];
            assert(info.is_closed());
            info.unmark_closed();
        }
    }

    do {
        timer.throw_if_expired();

        if (!check_and_solve(mdp, heuristic, current_trial_.back(), timer)) {
            break;
        }

        current_trial_.pop_back();
    } while (!current_trial_.empty());
}

template <typename State, typename Action, bool UseInterval>
bool LRTDP<State, Action, UseInterval>::check_and_solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    StateID init_state_id,
    utils::CountdownTimer& timer)
{
    assert(!current_trial_.empty() && policy_queue_.empty());

    ClearGuard guard(visited_);

    {
        StateInfo& state_info = this->state_infos_[init_state_id];
        if (state_info.is_solved()) return true;
        policy_queue_.emplace_back(init_state_id);
        state_info.mark_closed();
    }

    bool rv = true;

    do {
        timer.throw_if_expired();

        const auto state_id = policy_queue_.back();
        policy_queue_.pop_back();

        auto& info = this->state_infos_[state_id];
        assert(!info.is_solved());
        assert(info.is_closed());

        visited_.push_front(state_id);

        this->statistics_.check_and_solve_bellman_backups++;

        auto [value, transition] =
            this->compute_bellman_policy(mdp, heuristic, state_id, info);

        bool value_changed = this->update_value(info, value);
        this->update_policy(info, transition);

        if constexpr (UseInterval) {
            if (!info.bounds_agree()) {
                rv = false;
                continue;
            }
        } else {
            if (value_changed) {
                rv = false;
                continue;
            }
        }

        if (!transition) {
            continue;
        }

        for (StateID succ_id : transition->successor_dist.support()) {
            StateInfo& succ_info = this->state_infos_[succ_id];
            if (!succ_info.is_closed() && !succ_info.is_solved()) {
                succ_info.mark_closed();
                policy_queue_.emplace_back(succ_id);
            }
        }
    } while (!policy_queue_.empty());

    for (StateID sid : visited_) {
        StateInfo& info = this->state_infos_[sid];

        if (info.is_solved()) continue;

        assert(info.is_closed());
        info.unmark_closed();

        if (rv) {
            info.mark_solved();
        } else {
            statistics_.check_and_solve_bellman_backups++;
            auto [value, transition] =
                this->compute_bellman_policy(mdp, heuristic, sid, info);
            this->update_value(info, value);
            this->update_policy(info, transition);
        }
    }

    return rv;
}

} // namespace probfd::algorithms::lrtdp
