#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_LRTDP_H
#error "This file should only be included from lrtdp.h"
#endif

#include "probfd/algorithms/successor_sampler.h"

#include "downward/utils/countdown_timer.h"

#include <cassert>

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
    for (StateInfo& state_info : this->get_state_infos()) {
        state_info.clear();
    }
}

template <typename State, typename Action, bool UseInterval>
Interval LRTDP<State, Action, UseInterval>::do_solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> state,
    ProgressReport& progress,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    const StateID state_id = mdp.get_state_id(state);

    for (;;) {
        StateInfo& info = this->get_state_info(state_id);

        if (info.is_solved()) {
            break;
        }

        trial(mdp, heuristic, state_id, timer);
        this->statistics_.trials++;
        progress.print();
    }

    return this->lookup_bounds(state_id);
}

template <typename State, typename Action, bool UseInterval>
void LRTDP<State, Action, UseInterval>::print_additional_statistics(
    std::ostream& out) const
{
    statistics_.print(out);
}

template <typename State, typename Action, bool UseInterval>
void LRTDP<State, Action, UseInterval>::setup_custom_reports(
    param_type<State>,
    ProgressReport& progress)
{
    progress.register_print(
        [&](std::ostream& out) { out << "trials=" << statistics_.trials; });
}

template <typename State, typename Action, bool UseInterval>
void LRTDP<State, Action, UseInterval>::trial(
    MDPType& mdp,
    EvaluatorType& heuristic,
    StateID initial_state,
    utils::CountdownTimer& timer)
{
    using enum TrialTerminationCondition;

    ClearGuard guard(current_trial_);

    current_trial_.push_back(initial_state);

    for (;;) {
        timer.throw_if_expired();

        const StateID state_id = current_trial_.back();

        auto& state_info = this->get_state_info(state_id);
        if (state_info.is_solved()) {
            current_trial_.pop_back();
            break;
        }

        this->statistics_.trial_bellman_backups++;

        auto upd_info = this->bellman_policy_update(mdp, heuristic, state_id);
        const bool value_changed = upd_info.value_changed;
        const auto& transition = upd_info.greedy_transition;

        if (!transition) {
            // terminal
            assert(state_info.is_terminal());
            state_info.mark_solved();
            current_trial_.pop_back();
            break;
        }

        // state_info.mark_trial();
        assert(!state_info.is_terminal());

        if ((stop_consistent_ == CONSISTENT && !value_changed) ||
            (stop_consistent_ == INCONSISTENT && value_changed) ||
            (stop_consistent_ == REVISITED && state_info.is_marked_trial())) {
            break;
        }

        if (stop_consistent_ == REVISITED) {
            state_info.mark_trial();
        }

        auto next = sample_->sample(
            state_id,
            transition->action,
            transition->successor_dist,
            this->state_infos_);

        current_trial_.push_back(next);
    }

    if (stop_consistent_ == REVISITED) {
        current_trial_.pop_back();

        for (const StateID state : current_trial_) {
            auto& info = this->get_state_info(state);
            assert(info.is_marked_trial());
            info.unmark_trial();
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

    bool mark_solved = true;
    bool epsilon_consistent = true;

    {
        auto& init_info = this->get_state_info(init_state_id);
        if (init_info.is_solved()) return true;
        init_info.mark_open();
        policy_queue_.emplace_back(init_state_id);
    }

    do {
        timer.throw_if_expired();

        const auto state_id = policy_queue_.back();
        policy_queue_.pop_back();

        auto& info = this->get_state_info(state_id);
        assert(info.is_marked_open() && !info.is_solved());

        this->statistics_.check_and_solve_bellman_backups++;

        auto upd_info = this->bellman_policy_update(mdp, heuristic, state_id);
        const bool value_changed = upd_info.value_changed;
        const auto& transition = upd_info.greedy_transition;

        if (value_changed) {
            epsilon_consistent = false;
            visited_.push_front(state_id);
            continue;
        }

        if (!transition) {
            assert(info.is_terminal());
            info.mark_solved();
            continue;
        }

        visited_.push_front(state_id);

        if constexpr (UseInterval) {
            if (!info.bounds_agree()) {
                mark_solved = false;
            }
        }

        for (StateID succ_id : transition->successor_dist.support()) {
            auto& succ_info = this->get_state_info(succ_id);
            if (!succ_info.is_solved() && !succ_info.is_marked_open()) {
                succ_info.mark_open();
                policy_queue_.emplace_back(succ_id);
            }
        }
    } while (!policy_queue_.empty());

    if (epsilon_consistent && mark_solved) {
        for (StateID sid : visited_) {
            this->get_state_info(sid).mark_solved();
        }
    } else {
        for (StateID sid : visited_) {
            statistics_.check_and_solve_bellman_backups++;
            this->bellman_policy_update(mdp, heuristic, sid);
            this->get_state_info(sid).unmark();
        }
    }

    return epsilon_consistent && mark_solved;
}

/*
template <typename State, typename Action, bool UseInterval>
bool LRTDP<State, Action, UseInterval>::check_and_solve_original(
    MDP& mdp,
    Evaluator& heuristic,
    StateID init_state_id)
{
    bool rv = true;

    this->get_state_info(init_state_id).mark_open();
    policy_queue_.emplace_back(init_state_id);

    do {
        const StateID stateid = policy_queue_.back();
        policy_queue_.pop_back();
        visited_.push_back(stateid);

        auto& info = this->get_state_info(stateid);
        if (info.is_solved()) {
            continue;
        }

        this->statistics_.check_and_solve_bellman_backups++;
        const auto upd_info =
            this->bellman_policy_update(
                mdp,
                heuristic,
                stateid);

        const bool value_changed = upd_info.value_changed;
        const auto& std::optional transition = upd_info.transition;

        if (value_changed) {
            rv = false;
            continue;
        }

        if (rv && !transition) {
            for (StateID succid : transition->successor_dist.support()) {
                auto& succ_info = this->get_state_info(succid);
                if (!succ_info.is_solved() && !succ_info.is_marked_open()) {
                    succ_info.mark_open();
                    policy_queue_.emplace_back(succid);
                }
            }
        }
    } while (!policy_queue_.empty());

    if (rv) {
        while (!visited_.empty()) {
            const StateID sid = visited_.back();
            auto& info = this->get_state_info(sid);
            info.unmark();
            info.mark_solved();
            visited_.pop_back();
        }
    } else {
        while (!visited_.empty()) {
            const StateID sid = visited_.back();
            auto& info = this->get_state_info(sid);
            info.unmark();
            this->bellman_policy_update(mdp, heuristic, sid);
            visited_.pop_back();
            this->statistics_.check_and_solve_bellman_backups++;
        }
    }

    return rv;
}
*/

} // namespace probfd::algorithms::lrtdp
