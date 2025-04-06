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

inline bool DFSExplorationState::next_successor()
{
    successors.pop_back();
    return !successors.empty();
}

inline StateID DFSExplorationState::get_current_successor() const
{
    return successors.back();
}

} // namespace internal

template <typename State, typename Action, bool UseInterval>
HeuristicDepthFirstSearch<State, Action, UseInterval>::
    HeuristicDepthFirstSearch(
        value_t epsilon,
        std::shared_ptr<PolicyPicker> policy_chooser,
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool label_solved)
    : Base(epsilon, std::move(policy_chooser))
    , forward_updates_(forward_updates)
    , backtrack_update_type_(backtrack_update_type)
    , cutoff_tip_(cutoff_tip)
    , cutoff_inconsistent_(cutoff_inconsistent)
    , label_solved_(label_solved)
    , stack_index_(NEW)
{
}

template <typename State, typename Action, bool UseInterval>
Interval HeuristicDepthFirstSearch<State, Action, UseInterval>::do_solve(
    MDP& mdp,
    HeuristicType& heuristic,
    ParamType<State> state,
    ProgressReport& progress,
    double max_time)
{
    downward::utils::CountdownTimer timer(max_time);

    const StateID stateid = mdp.get_state_id(state);
    const StateInfo& state_info = this->state_infos_[stateid];

    progress.register_bound("v", [&state_info]() {
        return as_interval(state_info.value);
    });

    if (!label_solved_) {
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
        HeuristicType& heuristic,
        StateID stateid,
        ProgressReport& progress,
        downward::utils::CountdownTimer& timer)
{
    bool terminate;
    do {
        terminate = policy_exploration(mdp, heuristic, stateid, timer) &&
                    value_iteration(mdp, visited_states_, timer);

        visited_states_.clear();
        ++statistics_.iterations;
        progress.print();
    } while (!terminate);
}

template <typename State, typename Action, bool UseInterval>
void HeuristicDepthFirstSearch<State, Action, UseInterval>::
    solve_without_vi_termination(
        MDP& mdp,
        HeuristicType& heuristic,
        StateID stateid,
        ProgressReport& progress,
        downward::utils::CountdownTimer& timer)
{
    bool terminate;
    do {
        terminate = policy_exploration(mdp, heuristic, stateid, timer);
        ++statistics_.iterations;
        progress.print();
        assert(visited_states_.empty());
    } while (!terminate);
}

template <typename State, typename Action, bool UseInterval>
bool HeuristicDepthFirstSearch<State, Action, UseInterval>::policy_exploration(
    MDP& mdp,
    HeuristicType& heuristic,
    StateID state,
    downward::utils::CountdownTimer& timer)
{
    assert(visited_states_.empty());

    ClearGuard _(stack_index_);

    push(state);

    DFSExplorationState* einfo;
    StateInfo* sinfo;

    for (;;) {
        // DFS recursion
        do {
            einfo = &dfs_stack_.back();
            sinfo = &this->state_infos_[einfo->state_id];
        } while (initialize(mdp, heuristic, *einfo, *sinfo) &&
                 push_successor(mdp, *einfo, *sinfo, timer));

        // Iterative backtracking
        do {
            const uint32_t last_lowlink = einfo->lowlink;
            const bool backtrack_from_scc =
                last_lowlink == stack_index_[einfo->state_id];

            if (backtrack_from_scc) {
                auto scc = tarjan_stack_ | std::views::drop(last_lowlink);

                for (const StateID state_id : scc) {
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

            if (!backtrack_from_scc) {
                einfo->lowlink = std::min(last_lowlink, einfo->lowlink);
            }

            if (!bt_einfo.solved) einfo->solved = false;
        } while (!advance(mdp, *einfo, *sinfo) ||
                 !push_successor(mdp, *einfo, *sinfo, timer));
    }
}

template <typename State, typename Action, bool UseInterval>
bool HeuristicDepthFirstSearch<State, Action, UseInterval>::advance(
    MDP& mdp,
    DFSExplorationState& einfo,
    StateInfo& state_info)
{
    using enum BacktrackingUpdateType;

    if (einfo.next_successor()) {
        return true;
    }

    if (backtrack_update_type_ == SINGLE ||
        (backtrack_update_type_ == ON_DEMAND && !einfo.value_converged)) {
        const auto state = mdp.get_state(einfo.state_id);

        ClearGuard _(transitions_, qvalues_);
        this->generate_non_tip_transitions(mdp, state, transitions_);

        ++statistics_.backtracking_updates;

        auto value = this->compute_bellman_and_greedy(
            state,
            transitions_,
            mdp,
            qvalues_);

        auto transition = this->select_greedy_transition(
            mdp,
            state_info.get_policy(),
            transitions_);

        const auto val_upd =
            this->update_value(state_info, value, this->epsilon);
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
bool HeuristicDepthFirstSearch<State, Action, UseInterval>::push_successor(
    MDP& mdp,
    DFSExplorationState& einfo,
    StateInfo& sinfo,
    downward::utils::CountdownTimer& timer)
{
    using namespace internal;

    do {
        timer.throw_if_expired();

        const StateID succid = einfo.get_current_successor();
        const uint32_t succ_stack_index = stack_index_[succid];

        if (succ_stack_index == NEW) {
            push(succid);
            return true;
        } else if (succ_stack_index == CLOSED) {
            if (label_solved_) {
                einfo.solved =
                    einfo.solved && this->state_infos_[succid].is_solved();
            }
        } else {
            // is on stack
            einfo.lowlink = std::min(einfo.lowlink, succ_stack_index);
        }
    } while (advance(mdp, einfo, sinfo));

    return false;
}

template <typename State, typename Action, bool UseInterval>
void HeuristicDepthFirstSearch<State, Action, UseInterval>::push(
    StateID stateid)
{
    dfs_stack_.emplace_back(stateid, tarjan_stack_.size());
    stack_index_[stateid] = tarjan_stack_.size();
    tarjan_stack_.emplace_back(stateid);
}

template <typename State, typename Action, bool UseInterval>
bool HeuristicDepthFirstSearch<State, Action, UseInterval>::initialize(
    MDP& mdp,
    HeuristicType& heuristic,
    DFSExplorationState& einfo,
    StateInfo& sinfo)
{
    // Ignore labels if labelling option is turned off
    if (sinfo.is_solved()) {
        assert(label_solved_ || sinfo.is_goal_or_terminal());
        return false;
    }

    const auto state = mdp.get_state(einfo.state_id);

    const bool is_tip_state = sinfo.is_on_fringe();

    if (forward_updates_ || is_tip_state) {
        ClearGuard _(transitions_, qvalues_);

        if (is_tip_state) {
            this->expand_and_initialize(
                mdp,
                heuristic,
                state,
                sinfo,
                transitions_);
        } else {
            this->generate_non_tip_transitions(mdp, state, transitions_);
        }

        statistics_.forward_updates++;

        auto value = this->compute_bellman_and_greedy(
            state,
            transitions_,
            mdp,
            qvalues_);

        auto transition = this->select_greedy_transition(
            mdp,
            sinfo.get_policy(),
            transitions_);

        const auto val_upd = this->update_value(sinfo, value, this->epsilon);
        this->update_policy(sinfo, transition);

        einfo.value_converged = val_upd.converged;

        if (!transition) {
            return false;
        }

        const bool cutoff = (cutoff_tip_ && is_tip_state) ||
                            (cutoff_inconsistent_ && !val_upd.converged);

        if (cutoff) {
            einfo.solved = false;
            return false;
        }

        einfo.successors = std::ranges::to<std::vector>(
            transition->successor_dist.non_source_successor_dist.support());
    } else {
        const auto action = sinfo.get_policy();
        if (!action.has_value()) return false;

        ClearGuard _(successor_dist_.non_source_successor_dist);
        mdp.generate_action_transitions(state, *action, successor_dist_);

        einfo.successors = std::ranges::to<std::vector>(
            successor_dist_.non_source_successor_dist.support());
    }

    return true;
}

template <typename State, typename Action, bool UseInterval>
bool HeuristicDepthFirstSearch<State, Action, UseInterval>::value_iteration(
    MDP& mdp,
    const std::ranges::input_range auto& range,
    downward::utils::CountdownTimer& timer)
{
    ++statistics_.convergence_value_iterations;

    for (;;) {
        auto [value_changed, policy_changed] =
            vi_step(mdp, range, timer);

        if (policy_changed) return false;
        if (!value_changed) break;
    }

    return true;
}

template <typename State, typename Action, bool UseInterval>
std::pair<bool, bool>
HeuristicDepthFirstSearch<State, Action, UseInterval>::vi_step(
    MDP& mdp,
    const std::ranges::input_range auto& range,
    downward::utils::CountdownTimer& timer)
{
    bool values_not_conv = false;
    bool policy_not_conv = false;

    for (const StateID id : range) {
        timer.throw_if_expired();

        StateInfo& state_info = this->state_infos_[id];

        const auto state = mdp.get_state(id);

        ClearGuard _(transitions_, qvalues_);

        this->generate_non_tip_transitions(mdp, state, transitions_);

        const auto value = this->compute_bellman_and_greedy(
            state,
            transitions_,
            mdp,
            qvalues_);

        ++statistics_.convergence_updates;

        auto transition = this->select_greedy_transition(
            mdp,
            state_info.get_policy(),
            transitions_);

        auto val_upd = this->update_value(state_info, value, this->epsilon);
        bool policy_changed = this->update_policy(state_info, transition);
        values_not_conv = values_not_conv || !val_upd.converged;
        policy_not_conv = policy_not_conv || policy_changed;
    }

    return std::make_pair(values_not_conv, policy_not_conv);
}

} // namespace probfd::algorithms::heuristic_depth_first_search
