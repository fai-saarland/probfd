#ifndef GUARD_INCLUDE_PROBFD_PREPROCESSING_QUALITATIVE_REACHABILITY_ANALYSIS_H
#error                                                                         \
    "This file should only be included from qualitative_reachability_analysis.h"
#endif

#include "probfd/quotients/quotient_system.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/utils/iterators.h"

#include "probfd/evaluator.h"
#include "probfd/mdp.h"
#include "probfd/type_traits.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/timer.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <limits>
#include <type_traits>
#include <vector>

namespace probfd::preprocessing {

inline void QRStatistics::print(std::ostream& out) const
{
    out << "  Terminal states: " << terminals << " (" << goals
        << " goal states, " << selfloops << " self loop states)" << std::endl;
    out << "  Singleton SCC(s): " << sccs1 << " (" << sccs1_dead << " dead)"
        << std::endl;
    out << "  Non-singleton SCC(s): " << sccsk << " (" << sccsk_dead << " dead)"
        << std::endl;
    out << "  Qualitative reachability analysis: " << time << std::endl;
    out << "  " << ones << " state(s) can reach the goal with certainty"
        << std::endl;
}

namespace internal {

inline bool StateInfo::onstack() const
{
    return stackid < UNDEF;
}

inline auto StateInfo::get_status() const
{
    return explored ? (onstack() ? ONSTACK : CLOSED) : NEW;
}

inline StackInfo::StackInfo(StateID sid)
    : stateid(sid)
{
}

} // namespace internal

template <typename State, typename Action>
QualitativeReachabilityAnalysis<State, Action>::ExpansionInfo::ExpansionInfo(
    StateID state_id,
    StackInfo& stack_info,
    StateInfo& state_info,
    unsigned int stck)
    : state_id(state_id)
    , stack_info(stack_info)
    , state_info(state_info)
    , stck(stck)
    , lstck(stck)
{
}

template <typename State, typename Action>
bool QualitativeReachabilityAnalysis<State, Action>::ExpansionInfo::next_action(
    MDP& mdp)
{
    // Reset transition flags
    exits_only_solvable = true;
    transitions_in_scc = false;
    exits_scc = false;

    aops.pop_back();
    transition.clear();

    return !aops.empty() && forward_non_self_loop(mdp, mdp.get_state(state_id));
}

template <typename State, typename Action>
bool QualitativeReachabilityAnalysis<State, Action>::ExpansionInfo::
    forward_non_self_loop(MDP& mdp, const State& state)
{
    do {
        mdp.generate_action_transitions(state, aops.back(), transition);

        if (!transition.is_dirac(state_id)) {
            successor = transition.begin();
            return true;
        }

        aops.pop_back();
        transition.clear();
    } while (!aops.empty());

    return false;
}

template <typename State, typename Action>
bool QualitativeReachabilityAnalysis<State, Action>::ExpansionInfo::
    next_successor()
{
    if (++successor != transition.end()) {
        return true;
    }

    if (transitions_in_scc) {
        if (exits_only_solvable) {
            if (exits_scc) {
                ++stack_info.active_exit_transitions;
            }
            ++stack_info.active_transitions;
        }
        stack_info.transition_flags.emplace_back(
            exits_only_solvable && exits_scc,
            static_cast<bool>(exits_only_solvable));
    } else if (exits_only_solvable) {
        ++stack_info.active_exit_transitions;
        ++stack_info.active_transitions;
    }

    return false;
}

template <typename State, typename Action>
StateID QualitativeReachabilityAnalysis<State, Action>::ExpansionInfo::
    get_current_successor()
{
    return successor->item;
}

template <typename State, typename Action>
QualitativeReachabilityAnalysis<State, Action>::QualitativeReachabilityAnalysis(
    bool expand_goals)
    : expand_goals_(expand_goals)
{
}

template <typename State, typename Action>
void QualitativeReachabilityAnalysis<State, Action>::run_analysis(
    MDP& mdp,
    const Evaluator* pruning_function,
    param_type<State> source_state,
    std::output_iterator<StateID> auto dead_out,
    std::output_iterator<StateID> auto unsolvable_out,
    std::output_iterator<StateID> auto solvable_out,
    double max_time)
{
    assert(expansion_queue_.empty());

    utils::CountdownTimer timer(max_time);

    const StateID init_id = mdp.get_state_id(source_state);
    push_state(init_id, state_infos_[init_id]);

    for (;;) {
        ExpansionInfo* e;

        // DFS recursion
        do {
            e = &expansion_queue_.back();
        } while (initialize(mdp, pruning_function, *e) &&
                 push_successor(mdp, *e, timer));

        // Repeated backtracking
        do {
            timer.throw_if_expired();

            const unsigned stck = e->stck;
            const unsigned lstck = e->lstck;
            const bool backtrack_from_scc = stck == lstck;

            if (backtrack_from_scc) {
                scc_found(stck, dead_out, unsolvable_out, solvable_out, timer);
            }

            ExpansionInfo successor(std::move(*e));
            expansion_queue_.pop_back();

            if (expansion_queue_.empty()) return;

            e = &expansion_queue_.back();

            if (backtrack_from_scc) {
                if (!successor.state_info.solvable)
                    e->exits_only_solvable = false;
                e->exits_scc = true;
            } else {
                e->lstck = std::min(e->lstck, lstck);
                e->transitions_in_scc = true;

                successor.stack_info.parents.emplace_back(
                    e->stck,
                    e->stack_info.transition_flags.size());
            }

            if (!successor.state_info.dead) e->state_info.dead = false;
        } while ((!e->next_successor() && !e->next_action(mdp)) ||
                 !push_successor(mdp, *e, timer));
    }
}

template <typename State, typename Action>
bool QualitativeReachabilityAnalysis<State, Action>::initialize(
    MDP& mdp,
    const Evaluator* pruning_function,
    ExpansionInfo& exp_info)
{
    // assert(!state_info.explored);
    // assert(!state_info.solvable && state_info.dead);

    exp_info.state_info.explored = 1;

    const StateID state_id = exp_info.state_id;
    State state = mdp.get_state(state_id);

    const TerminationInfo term = mdp.get_termination_info(state);

    if (term.is_goal_state()) {
        ++stats_.goals;

        exp_info.state_info.dead = 0;
        ++exp_info.stack_info.active_exit_transitions;
        ++exp_info.stack_info.active_transitions;

        if (!expand_goals_) {
            ++stats_.terminals;
            return false;
        }
    } else if (
        pruning_function != nullptr &&
        pruning_function->evaluate(state) == term.get_cost()) {
        ++stats_.terminals;
        return false;
    }

    mdp.generate_applicable_actions(state, exp_info.aops);

    if (exp_info.aops.empty()) {
        ++stats_.terminals;
        return false;
    }

    return exp_info.forward_non_self_loop(mdp, state);
}

template <typename State, typename Action>
void QualitativeReachabilityAnalysis<State, Action>::push_state(
    StateID state_id,
    StateInfo& state_info)
{
    const std::size_t stack_size = stack_.size();
    state_info.stackid = stack_size;
    auto& stack_info = stack_.emplace_back(state_id);
    expansion_queue_.emplace_back(state_id, stack_info, state_info, stack_size);
}

template <typename State, typename Action>
bool QualitativeReachabilityAnalysis<State, Action>::push_successor(
    MDP& mdp,
    ExpansionInfo& exp_info,
    utils::CountdownTimer& timer)
{
    do {
        timer.throw_if_expired();

        const StateID succ_id = exp_info.get_current_successor();
        StateInfo& succ_info = state_infos_[succ_id];

        switch (succ_info.get_status()) {
        case StateInfo::NEW: {
            push_state(succ_id, succ_info);
            return true;
        }

        case StateInfo::CLOSED: // Child SCC
            exp_info.exits_only_solvable =
                exp_info.exits_only_solvable && succ_info.solvable;
            exp_info.exits_scc = true;
            exp_info.state_info.dead =
                exp_info.state_info.dead && succ_info.dead;
            break;

        case StateInfo::ONSTACK: // Same SCC
            unsigned succ_stack_id = succ_info.stackid;
            exp_info.lstck = std::min(exp_info.lstck, succ_stack_id);

            exp_info.transitions_in_scc = true;

            auto& parents = stack_[succ_stack_id].parents;
            parents.emplace_back(
                exp_info.stck,
                exp_info.stack_info.transition_flags.size());
        }
    } while (exp_info.next_successor() || exp_info.next_action(mdp));

    return false;
}

template <typename State, typename Action>
void QualitativeReachabilityAnalysis<State, Action>::scc_found(
    unsigned int stack_idx,
    std::output_iterator<StateID> auto dead_out,
    std::output_iterator<StateID> auto unsolvable_out,
    std::output_iterator<StateID> auto solvable_out,
    utils::CountdownTimer& timer)
{
    class Partition {
        std::vector<std::vector<int>::iterator> scc_index_to_local;
        std::vector<int> partition;
        std::vector<int>::iterator solvable_beg;
        std::vector<int>::iterator solvable_exits_beg;

    public:
        explicit Partition(std::size_t size)
            : scc_index_to_local(size)
            , partition(size, 0)
        {
            for (unsigned int i = 0; i != size; ++i) {
                scc_index_to_local[i] = partition.begin() + i;
                partition[i] = static_cast<int>(i);
            }

            solvable_beg = partition.begin();
            solvable_exits_beg = partition.begin();
        }

        auto solvable_begin() { return solvable_beg; }
        auto solvable_end() { return partition.end(); }

        auto solvable()
        {
            return std::ranges::subrange(solvable_begin(), solvable_end());
        }

        [[nodiscard]]
        bool has_solvable() const
        {
            return solvable_beg != partition.end();
        }

        void demote_unsolvable(int s)
        {
            auto local = scc_index_to_local[s];
            std::swap(scc_index_to_local[*solvable_beg], scc_index_to_local[s]);
            std::swap(*solvable_beg, *local);

            ++solvable_beg;
        }

        void demote_exit_unsolvable(int s)
        {
            auto local = scc_index_to_local[s];
            std::swap(
                scc_index_to_local[*solvable_exits_beg],
                scc_index_to_local[s]);
            std::swap(
                scc_index_to_local[*solvable_beg],
                scc_index_to_local[*solvable_exits_beg]);

            std::swap(*solvable_exits_beg, *local);
            std::swap(*solvable_beg, *solvable_exits_beg);

            ++solvable_beg;
            ++solvable_exits_beg;
        }

        void demote_exit_solvable(int s)
        {
            auto local = scc_index_to_local[s];
            std::swap(
                scc_index_to_local[*solvable_exits_beg],
                scc_index_to_local[s]);
            std::swap(*solvable_exits_beg, *local);

            ++solvable_exits_beg;
        }

        bool promote_solvable(int s)
        {
            if (!is_unsolvable(s)) {
                return false;
            }

            --solvable_beg;

            auto local = scc_index_to_local[s];
            std::swap(scc_index_to_local[*solvable_beg], scc_index_to_local[s]);
            std::swap(*solvable_beg, *local);

            return true;
        }

        void mark_non_exit_states_unsolvable()
        {
            solvable_beg = solvable_exits_beg;
        }

        bool is_unsolvable(int s)
        {
            return scc_index_to_local[s] < solvable_beg;
        }
    };

    using namespace std::views;

    auto scc = stack_ | std::views::drop(stack_idx);

    const StateInfo& st_info = state_infos_[std::ranges::begin(scc)->stateid];

    if (st_info.dead) {
        for (const StateID state_id : scc | transform(&StackInfo::stateid)) {
            StateInfo& info = state_infos_[state_id];
            info.stackid = StateInfo::UNDEF;
            assert(info.dead);
            *dead_out = state_id;
            *unsolvable_out = state_id;
        }

        stack_.erase(scc.begin(), scc.end());
        return;
    }

    // Compute the set of solvable states of this SCC.
    // Start by partitioning states into inactive states, active exits and
    // active non-exists.
    // The partition is initialized optimistically, i.e., all states start out
    // as active exits.
    Partition partition(scc.size());

    for (std::size_t i = 0; i != scc.size(); ++i) {
        StackInfo& info = scc[i];
        StateInfo& state_info = state_infos_[info.stateid];
        state_info.stackid = StateInfo::UNDEF;
        state_info.dead = false;

        assert(
            info.active_transitions != 0 || info.active_exit_transitions == 0);

        // Transform to local indices
        for (auto& parent_info : info.parents) {
            parent_info.parent_idx -= stack_idx;
        }

        if (info.active_exit_transitions == 0) {
            if (info.active_transitions > 0) {
                partition.demote_exit_solvable(i);
            } else {
                partition.demote_exit_unsolvable(i);
            }
        }
    }

    if (partition.has_solvable()) {
        // Compute the set of solvable states of this SCC.
        for (;;) {
            timer.throw_if_expired();

            // Collect states that can currently reach an exit and mark other
            // states unsolvable.
            auto unsolv_it = partition.solvable_begin();

            partition.mark_non_exit_states_unsolvable();

            for (auto it = partition.solvable_end();
                 it != partition.solvable_begin();) {
                for (const auto& [parent_idx, tr_idx] : scc[*--it].parents) {
                    StackInfo& pinfo = scc[parent_idx];

                    if (pinfo.transition_flags[tr_idx].is_active) {
                        partition.promote_solvable(parent_idx);
                    }
                }
            }

            // No new unsolvable states -> stop.
            if (unsolv_it == partition.solvable_begin()) break;

            // Run fixpoint iteration starting with the new unsolvable states
            // that could not reach an exit anymore.
            do {
                timer.throw_if_expired();

                StackInfo& scc_elem = scc[*unsolv_it];

                // The state was marked unsolvable.
                assert(partition.is_unsolvable(*unsolv_it));

                *unsolvable_out = scc_elem.stateid;

                for (const auto& [parent_idx, tr_idx] : scc_elem.parents) {
                    StackInfo& pinfo = scc[parent_idx];
                    auto& transition_flags = pinfo.transition_flags[tr_idx];

                    assert(
                        !transition_flags.is_active_exiting ||
                        transition_flags.is_active);

                    if (partition.is_unsolvable(parent_idx)) continue;

                    if (transition_flags.is_active_exiting) {
                        transition_flags.is_active_exiting = false;
                        transition_flags.is_active = false;

                        --pinfo.active_transitions;
                        --pinfo.active_exit_transitions;

                        if (pinfo.active_transitions == 0) {
                            partition.demote_exit_unsolvable(parent_idx);
                        } else if (pinfo.active_exit_transitions == 0) {
                            partition.demote_exit_solvable(parent_idx);
                        }
                    } else if (transition_flags.is_active) {
                        transition_flags.is_active = false;

                        --pinfo.active_transitions;

                        if (pinfo.active_transitions == 0) {
                            partition.demote_unsolvable(parent_idx);
                        }
                    }
                }
            } while (++unsolv_it != partition.solvable_begin());
        }
    }

    auto solvable = partition.solvable();
    stats_.ones += solvable.size();

    // Report the solvable states
    for (int scc_idx : solvable) {
        StackInfo& stkinfo = scc[scc_idx];
        const StateID sid = stkinfo.stateid;
        state_infos_[sid].solvable = true;
        *solvable_out = sid;
    }

    stack_.erase(scc.begin(), scc.end());
}

} // namespace probfd::preprocessing
