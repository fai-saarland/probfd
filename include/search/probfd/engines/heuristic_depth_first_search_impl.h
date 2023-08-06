#ifndef GUARD_INCLUDE_PROBFD_ENGINES_HEURISTIC_DEPTH_FIRST_SEARCH_H
#error "This file should only be included from heuristic_depth_first_search.h"
#endif

#include "downward/utils/countdown_timer.h"

#include <cassert>

namespace probfd {
namespace engines {
namespace heuristic_depth_first_search {

namespace internal {

inline void Statistics::print(std::ostream& out) const
{
    out << "  Iterations: " << iterations << std::endl;
    out << "  Value iterations (backtracking): "
        << backtracking_value_iterations << std::endl;
    out << "  Value iterations (convergence): " << convergence_value_iterations
        << std::endl;
    out << "  Bellman backups (forward): " << forward_updates << std::endl;
    out << "  Bellman backups (backtracking): " << backtracking_updates
        << std::endl;
    out << "  Bellman backups (convergence): " << convergence_updates
        << std::endl;
}

} // namespace internal

template <typename State, typename Action, bool UseInterval, bool Fret>
HeuristicDepthFirstSearch<State, Action, UseInterval, Fret>::
    HeuristicDepthFirstSearch(
        std::shared_ptr<PolicyPicker> policy_chooser,
        std::shared_ptr<NewStateObserver> new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
        bool LabelSolved,
        bool ForwardUpdates,
        BacktrackingUpdateType BackwardUpdates,
        bool CutoffInconsistent,
        bool GreedyExploration,
        bool PerformValueIteration,
        bool ExpandTipStates)
    : Base(policy_chooser, new_state_handler, report, interval_comparison)
    , LabelSolved(LabelSolved)
    , ForwardUpdates(ForwardUpdates)
    , BackwardUpdates(BackwardUpdates)
    , CutoffInconsistent(CutoffInconsistent)
    , GreedyExploration(GreedyExploration)
    , PerformValueIteration(PerformValueIteration)
    , ExpandTipStates(ExpandTipStates)
{
}

template <typename State, typename Action, bool UseInterval, bool Fret>
void HeuristicDepthFirstSearch<State, Action, UseInterval, Fret>::
    reset_search_state() 
{
    state_flags_.clear();
}

template <typename State, typename Action, bool UseInterval, bool Fret>
Interval HeuristicDepthFirstSearch<State, Action, UseInterval, Fret>::do_solve(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> state,
    double max_time) 
{
    utils::CountdownTimer timer(max_time);

    const StateID stateid = mdp.get_state_id(state);
    if (PerformValueIteration) {
        solve_with_vi_termination(mdp, heuristic, stateid, timer);
    } else {
        solve_without_vi_termination(mdp, heuristic, stateid, timer);
    }

    return this->get_state_info(stateid).get_bounds();
}

template <typename State, typename Action, bool UseInterval, bool Fret>
void HeuristicDepthFirstSearch<State, Action, UseInterval, Fret>::
    print_additional_statistics(std::ostream& out) const 
{
    statistics_.print(out);
}

template <typename State, typename Action, bool UseInterval, bool Fret>
auto HeuristicDepthFirstSearch<State, Action, UseInterval, Fret>::get_pers_info(
    StateID state_id) -> AdditionalStateInfo&
{
    if constexpr (Fret) {
        return state_flags_[state_id];
    } else {
        return this->get_state_info(state_id);
    }
}

template <typename State, typename Action, bool UseInterval, bool Fret>
void HeuristicDepthFirstSearch<State, Action, UseInterval, Fret>::
    solve_with_vi_termination(
        MDP& mdp,
        Evaluator& heuristic,
        StateID stateid,
        utils::CountdownTimer& timer)
{
    bool terminate = false;
    do {
        if (!policy_exploration<true>(mdp, heuristic, stateid, timer)) {
            unsigned ups = statistics_.backtracking_updates;
            statistics_.convergence_value_iterations++;
            auto x = value_iteration(mdp, heuristic, visited_, false, timer);
            terminate = !x.first && !x.second;
            statistics_.convergence_updates +=
                (statistics_.backtracking_updates - ups);
            statistics_.backtracking_updates = ups;
        }

        visited_.clear();
        statistics_.iterations++;
        this->print_progress();
    } while (!terminate);
}

template <typename State, typename Action, bool UseInterval, bool Fret>
void HeuristicDepthFirstSearch<State, Action, UseInterval, Fret>::
    solve_without_vi_termination(
        MDP& mdp,
        Evaluator& heuristic,
        StateID stateid,
        utils::CountdownTimer& timer)
{
    bool terminate = false;
    do {
        terminate = !policy_exploration<false>(mdp, heuristic, stateid, timer);
        statistics_.iterations++;
        this->print_progress();
        assert(visited_.empty());
    } while (!terminate);
}

template <typename State, typename Action, bool UseInterval, bool Fret>
template <bool GetVisited>
bool HeuristicDepthFirstSearch<State, Action, UseInterval, Fret>::
    policy_exploration(
        MDP& mdp,
        Evaluator& heuristic,
        StateID state,
        utils::CountdownTimer& timer)
{
    using enum BacktrackingUpdateType;

    ClearGuard _guard(state_infos_);

    {
        AdditionalStateInfo& pers_info = get_pers_info(state);
        bool value_changed = false;
        bool pruned = false;
        const uint8_t pstatus =
            push(mdp, heuristic, state, pers_info, value_changed, pruned);

        if (pers_info.is_solved()) {
            // is terminal
            return false;
        }

        if (pstatus != LocalStateInfo::ONSTACK) {
            return value_changed || pruned;
        }

        stack_.push_back(state);
        state_infos_[state].open(0);
    }

    bool keep_expanding = true;

    ExpansionInfo* einfo = &expansion_queue_.back();
    LocalStateInfo* sinfo = &state_infos_[einfo->stateid];

    for (;;) {
    recurse:;
        timer.throw_if_expired();

        while (keep_expanding && !einfo->successors.empty()) {
            timer.throw_if_expired();

            StateID succid = einfo->successors.back();
            einfo->successors.pop_back();

            AdditionalStateInfo& pers_succ_info = get_pers_info(succid);

            if (pers_succ_info.is_solved()) {
                if (!this->is_marked_dead_end(succid)) {
                    einfo->dead = false;
                }

                einfo->leaf = false;
                continue;
            }

            LocalStateInfo& succ_info = state_infos_[succid];
            if (succ_info.status == LocalStateInfo::NEW) {
                const uint8_t status = push(
                    mdp,
                    heuristic,
                    succid,
                    pers_succ_info,
                    einfo->value_changed,
                    einfo->unsolved_successor);

                if (status == LocalStateInfo::ONSTACK) {
                    succ_info.open(stack_.size());
                    stack_.push_back(succid);
                    einfo = &expansion_queue_.back();
                    sinfo = &state_infos_[einfo->stateid];
                    goto recurse;
                }

                einfo->leaf = false;
                succ_info.status = status;

                if (status == LocalStateInfo::UNSOLVED) {
                    einfo->unsolved_successor = true;
                    einfo->dead = false;
                } else if (status == LocalStateInfo::CLOSED) {
                    einfo->dead = false;
                }

                if (GreedyExploration && einfo->unsolved_successor) {
                    keep_expanding = false;
                }
            } else if (succ_info.status == LocalStateInfo::ONSTACK) {
                sinfo->lowlink = std::min(sinfo->lowlink, succ_info.index);
            } else {
                einfo->unsolved_successor =
                    einfo->unsolved_successor ||
                    succ_info.status == LocalStateInfo::UNSOLVED;
                einfo->leaf = false;
                einfo->dead = einfo->dead &&
                              succ_info.status == LocalStateInfo::CLOSED_DEAD;
            }
        }

        unsigned last_lowlink = sinfo->lowlink;
        bool last_unsolved_successors = einfo->unsolved_successor;
        bool last_dead = einfo->dead;
        bool last_value_changed = einfo->value_changed;
        bool last_leaf = einfo->leaf;

        if (BackwardUpdates == SINGLE ||
            (last_value_changed && BackwardUpdates == ON_DEMAND)) {
            statistics_.backtracking_updates++;
            auto result =
                this->bellman_policy_update(mdp, heuristic, einfo->stateid);
            last_value_changed = result.value_changed;
            last_unsolved_successors =
                last_unsolved_successors || result.policy_changed;
        }

        if (sinfo->index == sinfo->lowlink) {
            last_leaf = false;

            auto scc = std::ranges::subrange(
                std::next(stack_.begin(), sinfo->index),
                stack_.end());

            for (const StateID state_id : scc) {
                state_infos_[state_id].status = LocalStateInfo::UNSOLVED;
            }

            if constexpr (GetVisited) {
                if (!last_unsolved_successors && !last_value_changed) {
                    visited_.insert(visited_.end(), scc.begin(), scc.end());
                }
            }

            if (BackwardUpdates == CONVERGENCE && last_unsolved_successors) {
                auto result = value_iteration(mdp, heuristic, scc, true, timer);
                last_value_changed = result.first;
                last_unsolved_successors =
                    result.second || last_unsolved_successors;
            }

            last_dead = false;

            last_unsolved_successors =
                last_unsolved_successors || last_value_changed;

            if (!last_unsolved_successors) {
                const uint8_t closed = last_dead ? LocalStateInfo::CLOSED_DEAD
                                                 : LocalStateInfo::CLOSED;
                for (const StateID state_id : scc) {
                    state_infos_[state_id].status = closed;

                    if (LabelSolved) {
                        get_pers_info(state_id).set_solved();
                    }
                }
            }

            stack_.erase(scc.begin(), scc.end());
        }

        expansion_queue_.pop_back();

        if (expansion_queue_.empty())
            return last_unsolved_successors || last_value_changed;

        einfo = &expansion_queue_.back();
        sinfo = &state_infos_[einfo->stateid];

        sinfo->lowlink = std::min(sinfo->lowlink, last_lowlink);
        einfo->unsolved_successor =
            einfo->unsolved_successor || last_unsolved_successors;
        einfo->value_changed = einfo->value_changed || last_value_changed;
        einfo->dead = einfo->dead && last_dead;
        einfo->leaf = einfo->leaf && last_leaf;
    }
}

template <typename State, typename Action, bool UseInterval, bool Fret>
uint8_t HeuristicDepthFirstSearch<State, Action, UseInterval, Fret>::push(
    MDP& mdp,
    Evaluator& heuristic,
    StateID stateid,
    AdditionalStateInfo& sinfo,
    bool& parent_value_changed,
    bool& parent_unsolved_successors)
{
    assert(!sinfo.is_solved());

    const bool is_tip_state = !sinfo.is_policy_initialized();

    if (ForwardUpdates || is_tip_state) {
        sinfo.set_policy_initialized();
        statistics_.forward_updates++;
        const auto upd_info =
            this->bellman_policy_update(mdp, heuristic, stateid);
        const bool value_changed = upd_info.value_changed;
        const auto& transition = upd_info.greedy_transition;

        if constexpr (UseInterval) {
            parent_value_changed = parent_value_changed || value_changed ||
                                   (this->interval_comparison_ &&
                                    !this->get_state_info(stateid, sinfo)
                                         .value.bounds_approximately_equal());
        } else {
            parent_value_changed = parent_value_changed || value_changed;
        }

        if (!transition) {
            sinfo.set_solved();
            uint8_t closed = LocalStateInfo::CLOSED;
            if (this->notify_dead_end_ifnot_goal(stateid)) {
                closed = LocalStateInfo::CLOSED_DEAD;
            }

            return value_changed ? LocalStateInfo::UNSOLVED : closed;
        }

        if ((!ExpandTipStates && is_tip_state) ||
            (CutoffInconsistent && value_changed)) {
            parent_unsolved_successors = true;

            return LocalStateInfo::UNSOLVED;
        }

        auto& einfo =
            expansion_queue_.emplace_back(stateid, transition->successor_dist);
        einfo.value_changed = value_changed;
    } else {
        if (this->get_state_info(stateid, sinfo).is_dead_end()) {
            sinfo.set_solved();
            return LocalStateInfo::CLOSED_DEAD;
        }

        const auto transition =
            this->bellman_policy_update(mdp, heuristic, stateid)
                .greedy_transition;
        assert(transition.has_value());
        expansion_queue_.emplace_back(stateid, transition->successor_dist);
    }

    return LocalStateInfo::ONSTACK;
}

template <typename State, typename Action, bool UseInterval, bool Fret>
std::pair<bool, bool>
HeuristicDepthFirstSearch<State, Action, UseInterval, Fret>::value_iteration(
    MDP& mdp,
    Evaluator& heuristic,
    const std::ranges::input_range auto& range,
    bool until_convergence,
    utils::CountdownTimer& timer)
{
    std::pair<bool, bool> updated_all(false, false);
    bool value_changed = true;
    bool policy_graph_changed;

    do {
        bool all_converged = true;

        value_changed = false;
        policy_graph_changed = false;

        for (const StateID id : range) {
            timer.throw_if_expired();

            statistics_.backtracking_updates++;

            const auto result = this->bellman_policy_update(mdp, heuristic, id);
            value_changed = value_changed || result.value_changed;

            if constexpr (UseInterval) {
                all_converged =
                    all_converged && (!this->interval_comparison_ ||
                                      this->get_state_info(id)
                                          .value.bounds_approximately_equal());
            }

            policy_graph_changed =
                policy_graph_changed || result.policy_changed;
        }

        updated_all.first =
            updated_all.first || value_changed || !all_converged;
        updated_all.second = updated_all.second || policy_graph_changed;
    } while (value_changed && (until_convergence || !policy_graph_changed));

    return updated_all;
}

} // namespace heuristic_depth_first_search
} // namespace engines
} // namespace probfd