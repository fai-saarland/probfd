#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H
#error "This file should only be included from heuristic_depth_first_search.h"
#endif

#include "downward/utils/countdown_timer.h"

#include <cassert>

namespace probfd::algorithms::heuristic_depth_first_search {

namespace internal {

inline void Statistics::print(std::ostream& out) const
{
    out << "  Iterations: " << iterations << std::endl;
    out << "  Value iterations: " << convergence_value_iterations << std::endl;
    out << "  Bellman backups (forward): " << forward_updates << std::endl;
    out << "  Bellman backups (backtracking): " << backtracking_updates
        << std::endl;
    out << "  Bellman backups (convergence): " << convergence_updates
        << std::endl;
}

bool ExpansionInfo::next_successor()
{
    successors.pop_back();
    return !successors.empty();
}

StateID ExpansionInfo::get_current_successor() const
{
    return successors.back();
}

} // namespace internal

template <typename State, typename Action, bool UseInterval>
HeuristicDepthFirstSearch<State, Action, UseInterval>::
    HeuristicDepthFirstSearch(
        std::shared_ptr<PolicyPicker> policy_chooser,
        bool label_solved,
        bool forward_updates,
        BacktrackingUpdateType backward_updates,
        bool cutoff_inconsistent,
        bool greedy_exploration,
        bool perform_value_iteration,
        bool cutoff_tip)
    : Base(std::move(policy_chooser))
    , label_solved_(label_solved)
    , forward_updates_(forward_updates)
    , backward_updates_(backward_updates)
    , cutoff_inconsistent_(cutoff_inconsistent)
    , greedy_exploration_(greedy_exploration)
    , perform_value_iteration_(perform_value_iteration)
    , cutoff_tip_(cutoff_tip)
{
}

template <typename State, typename Action, bool UseInterval>
void HeuristicDepthFirstSearch<State, Action, UseInterval>::reset_search_state()
{
    this->state_infos_.reset();
}

template <typename State, typename Action, bool UseInterval>
Interval HeuristicDepthFirstSearch<State, Action, UseInterval>::do_solve(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> state,
    ProgressReport& progress,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    const StateID stateid = mdp.get_state_id(state);
    const StateInfo& state_info = this->state_infos_[stateid];

    progress.register_bound("v", [&state_info]() {
        return as_interval(state_info.value);
    });

    if (perform_value_iteration_) {
        solve_with_vi_termination(mdp, heuristic, stateid, progress, timer);
    } else {
        solve_without_vi_termination(mdp, heuristic, stateid, progress, timer);
    }

    return state_info.get_bounds();
}

template <typename State, typename Action, bool UseInterval>
void HeuristicDepthFirstSearch<State, Action, UseInterval>::
    print_additional_statistics(std::ostream& out) const
{
    statistics_.print(out);
}

template <typename State, typename Action, bool UseInterval>
void HeuristicDepthFirstSearch<State, Action, UseInterval>::
    solve_with_vi_termination(
        MDP& mdp,
        Evaluator& heuristic,
        StateID stateid,
        ProgressReport& progress,
        utils::CountdownTimer& timer)
{
    bool terminate = false;
    do {
        terminate = policy_exploration<true>(mdp, heuristic, stateid, timer) &&
                    value_iteration(mdp, heuristic, visited_, timer);

        visited_.clear();
        ++statistics_.iterations;
        progress.print();
    } while (!terminate);
}

template <typename State, typename Action, bool UseInterval>
void HeuristicDepthFirstSearch<State, Action, UseInterval>::
    solve_without_vi_termination(
        MDP& mdp,
        Evaluator& heuristic,
        StateID stateid,
        ProgressReport& progress,
        utils::CountdownTimer& timer)
{
    bool terminate = false;
    do {
        terminate = policy_exploration<false>(mdp, heuristic, stateid, timer);
        ++statistics_.iterations;
        progress.print();
        assert(visited_.empty());
    } while (!terminate);
}

template <typename State, typename Action, bool UseInterval>
template <bool GetVisited>
bool HeuristicDepthFirstSearch<State, Action, UseInterval>::policy_exploration(
    MDP& mdp,
    Evaluator& heuristic,
    StateID state,
    utils::CountdownTimer& timer)
{
    using namespace internal;
    using enum BacktrackingUpdateType;

    ClearGuard _(local_state_infos_);

    {
        StateInfo& pers_info = this->state_infos_[state];

        if (pers_info.is_solved()) {
            return true;
        }

        bool value_changed = false;
        const uint8_t pstatus =
            push(mdp, heuristic, state, pers_info, value_changed);

        if (pers_info.is_solved()) {
            // is terminal
            return true;
        }

        if (pstatus != ONSTACK) {
            return !value_changed && pstatus != UNSOLVED;
        }

        stack_.push_back(state);
        local_state_infos_[state].open(0);
    }

    bool keep_expanding = true;

    ExpansionInfo* einfo = &expansion_queue_.back();
    LocalStateInfo* sinfo = &local_state_infos_[einfo->stateid];

    for (;;) {
        // DFS recursion
        while (this->push_successor(
            mdp,
            heuristic,
            *einfo,
            *sinfo,
            keep_expanding,
            timer)) {
            einfo = &expansion_queue_.back();
            sinfo = &local_state_infos_[einfo->stateid];
        }

        // Iterative backtracking
        do {
            unsigned last_lowlink = sinfo->lowlink;
            bool last_unsolved_succs = einfo->unsolved_succs;
            bool last_value_changed = einfo->value_changed;

            if (backward_updates_ == SINGLE ||
                (backward_updates_ == ON_DEMAND && last_value_changed)) {
                statistics_.backtracking_updates++;
                StateInfo& state_info = this->state_infos_[einfo->stateid];
                auto [value, transition] = this->compute_bellman_policy(
                    mdp,
                    heuristic,
                    einfo->stateid,
                    state_info);
                bool value_changed = this->update_value(state_info, value);
                bool policy_changed =
                    this->update_policy(state_info, transition);
                last_value_changed = value_changed;
                last_unsolved_succs = last_unsolved_succs || policy_changed;
            }

            if (sinfo->index == sinfo->lowlink) {
                auto scc = stack_ | std::views::drop(sinfo->index);

                for (const StateID state_id : scc) {
                    local_state_infos_[state_id].status = UNSOLVED;
                }

                if constexpr (GetVisited) {
                    if (!last_unsolved_succs && !last_value_changed) {
                        visited_.insert(visited_.end(), scc.begin(), scc.end());
                    }
                }

                last_unsolved_succs = last_unsolved_succs || last_value_changed;

                if (!last_unsolved_succs) {
                    for (const StateID state_id : scc) {
                        local_state_infos_[state_id].status = CLOSED;

                        if (label_solved_) {
                            this->state_infos_[state_id].set_solved();
                        }
                    }
                }

                stack_.erase(scc.begin(), scc.end());
            }

            expansion_queue_.pop_back();

            if (expansion_queue_.empty())
                return !last_unsolved_succs && !last_value_changed;

            einfo = &expansion_queue_.back();
            sinfo = &local_state_infos_[einfo->stateid];

            sinfo->lowlink = std::min(sinfo->lowlink, last_lowlink);
            einfo->unsolved_succs =
                einfo->unsolved_succs || last_unsolved_succs;
            einfo->value_changed = einfo->value_changed || last_value_changed;
        } while (!einfo->next_successor() || !keep_expanding);
    }
}

template <typename State, typename Action, bool UseInterval>
bool HeuristicDepthFirstSearch<State, Action, UseInterval>::push_successor(
    MDP& mdp,
    Evaluator& heuristic,
    ExpansionInfo& einfo,
    LocalStateInfo& sinfo,
    bool& keep_expanding,
    utils::CountdownTimer& timer)
{
    using namespace internal;

    do {
        timer.throw_if_expired();

        StateID succid = einfo.get_current_successor();

        StateInfo& pers_succ_info = this->state_infos_[succid];

        if (pers_succ_info.is_solved()) {
            continue;
        }

        LocalStateInfo& succ_info = local_state_infos_[succid];
        if (succ_info.status == NEW) {
            const uint8_t status = push(
                mdp,
                heuristic,
                succid,
                pers_succ_info,
                einfo.value_changed);

            if (status == ONSTACK) {
                succ_info.open(stack_.size());
                stack_.push_back(succid);
                return true;
            }

            succ_info.status = status;

            if (status == UNSOLVED) {
                einfo.unsolved_succs = true;
            }

            if (greedy_exploration_ && einfo.unsolved_succs) {
                keep_expanding = false;
                break;
            }
        } else if (succ_info.status == ONSTACK) {
            sinfo.lowlink = std::min(sinfo.lowlink, succ_info.index);
        } else {
            einfo.unsolved_succs =
                einfo.unsolved_succs || succ_info.status == UNSOLVED;
        }
    } while (einfo.next_successor());

    return false;
}

template <typename State, typename Action, bool UseInterval>
uint8_t HeuristicDepthFirstSearch<State, Action, UseInterval>::push(
    MDP& mdp,
    Evaluator& heuristic,
    StateID stateid,
    StateInfo& sinfo,
    bool& parent_value_changed)
{
    using namespace internal;

    assert(!sinfo.is_solved());

    const bool is_tip_state = sinfo.is_on_fringe();

    if (forward_updates_ || is_tip_state) {
        statistics_.forward_updates++;

        auto [value, transition] =
            this->compute_bellman_policy(mdp, heuristic, stateid, sinfo);

        bool value_changed = this->update_value(sinfo, value);
        this->update_policy(sinfo, transition);

        parent_value_changed = parent_value_changed || value_changed;

        if constexpr (UseInterval) {
            parent_value_changed = parent_value_changed ||
                                   !sinfo.value.bounds_approximately_equal();
        }

        if (!transition) {
            sinfo.set_solved();
            return CLOSED;
        }

        const bool cutoff = (cutoff_tip_ && is_tip_state) ||
                            (cutoff_inconsistent_ && value_changed);

        if (cutoff) {
            return UNSOLVED;
        }

        auto& einfo =
            expansion_queue_.emplace_back(stateid, transition->successor_dist);
        einfo.value_changed = value_changed;
    } else {
        const auto action = sinfo.get_policy();
        assert(action.has_value());
        const State state = mdp.get_state(stateid);
        Distribution<StateID> successor_dist;
        mdp.generate_action_transitions(state, *action, successor_dist);
        expansion_queue_.emplace_back(stateid, successor_dist);
    }

    return ONSTACK;
}

template <typename State, typename Action, bool UseInterval>
bool HeuristicDepthFirstSearch<State, Action, UseInterval>::value_iteration(
    MDP& mdp,
    Evaluator& heuristic,
    const std::ranges::input_range auto& range,
    utils::CountdownTimer& timer)
{
    ++statistics_.convergence_value_iterations;

    for (;;) {
        auto [value_changed, policy_changed] = vi_step(
            mdp,
            heuristic,
            range,
            timer,
            statistics_.convergence_updates);

        if (policy_changed) return false;
        if (!value_changed) break;
    }

    return true;
}

template <typename State, typename Action, bool UseInterval>
std::pair<bool, bool>
HeuristicDepthFirstSearch<State, Action, UseInterval>::vi_step(
    MDP& mdp,
    Evaluator& heuristic,
    const std::ranges::input_range auto& range,
    utils::CountdownTimer& timer,
    unsigned long long& stat_counter)
{
    bool values_not_conv = false;
    bool policy_not_conv = false;

    for (const StateID id : range) {
        timer.throw_if_expired();

        ++stat_counter;

        StateInfo& state_info = this->state_infos_[id];
        const auto [value, transition] =
            this->compute_bellman_policy(mdp, heuristic, id, state_info);
        bool value_changed = this->update_value(state_info, value);
        bool policy_changed = this->update_policy(state_info, transition);
        values_not_conv = values_not_conv || value_changed;
        policy_not_conv = policy_not_conv || policy_changed;

        if constexpr (UseInterval) {
            values_not_conv = values_not_conv ||
                              !state_info.value.bounds_approximately_equal();
        }
    }

    return std::make_pair(values_not_conv, policy_not_conv);
}

} // namespace probfd::algorithms::heuristic_depth_first_search
