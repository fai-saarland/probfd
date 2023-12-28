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

namespace probfd {

namespace preprocessing {

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
    return stackid_ < UNDEF;
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
    unsigned int stck)
    : stck(stck)
    , lstck(stck)
{
}

template <typename State, typename Action>
bool QualitativeReachabilityAnalysis<State, Action>::ExpansionInfo::next_action(
    MDP& mdp,
    StateID state_id)
{
    // Reset transition flags
    exits_only_solvable = true;
    transitions_in_scc = false;
    exits_scc = false;

    aops.pop_back();
    transition.clear();

    return !aops.empty() &&
           forward_non_self_loop(mdp, mdp.get_state(state_id), state_id);
}

template <typename State, typename Action>
bool QualitativeReachabilityAnalysis<State, Action>::ExpansionInfo::
    forward_non_self_loop(MDP& mdp, const State& state, StateID state_id)
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
    next_successor(StackInfo& s)
{
    if (++successor != transition.end()) {
        return true;
    }

    if (transitions_in_scc) {
        if (exits_only_solvable) {
            if (exits_scc) {
                ++s.active_exit_transitions;
            }
            ++s.active_transitions;
        }
        s.transition_flags.emplace_back(
            exits_only_solvable && exits_scc,
            exits_only_solvable);
    } else if (exits_only_solvable) {
        ++s.active_exit_transitions;
        ++s.active_transitions;
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
    state_infos_[init_id].stackid_ = 0;
    stack_.emplace_back(init_id);
    expansion_queue_.emplace_back(0);

    ExpansionInfo* e = &expansion_queue_.back();
    StackInfo* s = &stack_[e->stck];
    StateInfo* st = &state_infos_[s->stateid];

    goto start;

    for (;;) {
        // DFS recursion
        while (push_successor(mdp, *e, *s, *st, timer)) {
            e = &expansion_queue_.back();
            s = &stack_[e->stck];
            st = &state_infos_[s->stateid];
        start:
            if (!initialize(mdp, pruning_function, *e, *s, *st)) break;
        }

        // Repeated backtracking
        do {
            const unsigned stck = e->stck;
            const unsigned lstck = e->lstck;
            const bool scc_root = stck == lstck;

            if (scc_root) {
                scc_found(
                    stack_ | std::views::drop(stck),
                    dead_out,
                    unsolvable_out,
                    solvable_out,
                    timer);
            }

            expansion_queue_.pop_back();

            if (expansion_queue_.empty()) {
                return;
            }

            timer.throw_if_expired();

            StateInfo& bt_info = state_infos_[s->stateid];
            StackInfo* backtracked_from = s;

            e = &expansion_queue_.back();
            s = &stack_[e->stck];
            st = &state_infos_[s->stateid];

            // Backtracked from successor.
            if (scc_root) { // Child SCC
                e->exits_only_solvable =
                    e->exits_only_solvable && bt_info.solvable;
                e->exits_scc = true;
            } else { // Same SCC
                e->lstck = std::min(e->lstck, lstck);
                e->transitions_in_scc = true;

                auto& parents = backtracked_from->parents;
                parents.emplace_back(e->stck, s->transition_flags.size());
            }

            st->dead = st->dead && bt_info.dead;
        } while (!e->next_successor(*s) && !e->next_action(mdp, s->stateid));
    }
}

template <typename State, typename Action>
bool QualitativeReachabilityAnalysis<State, Action>::initialize(
    MDP& mdp,
    const Evaluator* pruning_function,
    ExpansionInfo& exp_info,
    StackInfo& stack_info,
    StateInfo& state_info)
{
    assert(!state_info.explored);
    assert(!state_info.solvable && state_info.dead);

    state_info.explored = 1;

    const StateID state_id = stack_info.stateid;
    State state = mdp.get_state(state_id);

    const TerminationInfo term = mdp.get_termination_info(state);

    if (term.is_goal_state()) {
        ++stats_.goals;

        state_info.dead = 0;
        ++stack_info.active_exit_transitions;
        ++stack_info.active_transitions;

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

    return exp_info.forward_non_self_loop(mdp, state, state_id);
}

template <typename State, typename Action>
bool QualitativeReachabilityAnalysis<State, Action>::push_successor(
    MDP& mdp,
    ExpansionInfo& e,
    StackInfo& s,
    StateInfo& st,
    utils::CountdownTimer& timer)
{
    do {
        timer.throw_if_expired();

        const StateID succ_id = e.get_current_successor();
        StateInfo& succ_info = state_infos_[succ_id];

        switch (succ_info.get_status()) {
        case StateInfo::NEW: {
            const std::size_t stack_size = stack_.size();
            succ_info.stackid_ = stack_size;
            stack_.emplace_back(succ_id);
            expansion_queue_.emplace_back(stack_size);

            return true;
        }

        case StateInfo::CLOSED: // Child SCC
            e.exits_only_solvable = e.exits_only_solvable && succ_info.solvable;
            e.exits_scc = true;
            break;

        case StateInfo::ONSTACK: // Same SCC
            unsigned succ_stack_id = succ_info.stackid_;
            e.lstck = std::min(e.lstck, succ_stack_id);

            e.transitions_in_scc = true;

            auto& parents = stack_[succ_stack_id].parents;
            parents.emplace_back(e.stck, s.transition_flags.size());
        }

        st.dead = st.dead && succ_info.dead;
    } while (e.next_successor(s) || e.next_action(mdp, s.stateid));

    return false;
}

template <typename State, typename Action>
void QualitativeReachabilityAnalysis<State, Action>::scc_found(
    std::ranges::forward_range auto&& scc,
    std::output_iterator<StateID> auto dead_out,
    std::output_iterator<StateID> auto unsolvable_out,
    std::output_iterator<StateID> auto solvable_out,
    utils::CountdownTimer& timer)
{
    using namespace std::views;

    const StateInfo& st_info = state_infos_[std::ranges::begin(scc)->stateid];

    if (st_info.dead) {
        for (const StateID state_id : scc | transform(&StackInfo::stateid)) {
            StateInfo& info = state_infos_[state_id];
            info.stackid_ = StateInfo::UNDEF;
            assert(info.dead);
            *dead_out = state_id;
            *unsolvable_out = state_id;
        }

        stack_.erase(scc.begin(), scc.end());
        return;
    }

    // Compute the set of solvable states of this SCC.
    // Start by assuming all states are solvable.
    // Also collect the set of "trusted" states. A trusted state is a goal
    // state or a state that has a transition that only goes to states labelled
    // solvable and can leave the SCC.

    std::set<StackInfo*> solvable_states;
    std::set<StackInfo*> trusted;

    for (StackInfo& info : scc) {
        solvable_states.insert(&info);

        StateInfo& state_info = state_infos_[info.stateid];
        state_info.stackid_ = StateInfo::UNDEF;
        state_info.dead = false;

        if (info.active_exit_transitions > 0) {
            trusted.insert(&info);
        }
    }

    std::size_t prev_size;
    std::size_t current_size = solvable_states.size();

    // Compute the set of solvable states of this SCC.
    do {
        timer.throw_if_expired();

        // Collect states that can currently reach a trusted state.
        std::set<StackInfo*> can_reach_trusted = trusted;

        {
            std::vector<StackInfo*> queue(trusted.begin(), trusted.end());

            while (!queue.empty()) {
                StackInfo* scc_elem = queue.back();
                queue.pop_back();

                for (const auto& [parent_idx, tr_idx] : scc_elem->parents) {
                    StackInfo& pinfo = stack_[parent_idx];

                    if (pinfo.transition_flags[tr_idx].is_active &&
                        can_reach_trusted.insert(&pinfo).second) {
                        queue.push_back(&pinfo);
                    }
                }
            }
        }

        // The complement is unsolvable.
        std::set<StackInfo*> proven_unsolvable;
        std::ranges::set_difference(
            solvable_states,
            can_reach_trusted,
            std::inserter(proven_unsolvable, proven_unsolvable.end()));

        // Iteratively prune unsolvable states and transitions which
        // have these states as a target until a fixpoint is reached.
        // If a state has no active transitions left, it is labelled as
        // unsolvable as well and is added to the queue.
        std::vector<StackInfo*> queue(
            proven_unsolvable.begin(),
            proven_unsolvable.end());

        while (!queue.empty()) {
            timer.throw_if_expired();

            StackInfo* scc_elem = queue.back();
            queue.pop_back();

            // The state is unsolvable.
            assert(solvable_states.find(scc_elem) != solvable_states.end());
            solvable_states.erase(scc_elem);

            *unsolvable_out = scc_elem->stateid;

            for (const auto& [parent_idx, tr_idx] : scc_elem->parents) {
                StackInfo& pinfo = stack_[parent_idx];
                auto& transition_flags = pinfo.transition_flags[tr_idx];

                if (proven_unsolvable.contains(&pinfo)) {
                    continue;
                }

                if (transition_flags.is_active_exiting) {
                    transition_flags.is_active_exiting = false;

                    if (--pinfo.active_exit_transitions == 0) {
                        trusted.erase(&pinfo);
                    }
                } else if (!transition_flags.is_active) {
                    continue;
                }

                assert(transition_flags.is_active);
                transition_flags.is_active = false;

                if (--pinfo.active_transitions == 0) {
                    queue.push_back(&pinfo);
                }
            }
        }

        prev_size = current_size;
        current_size = solvable_states.size();
    } while (prev_size != current_size);

    stats_.ones += solvable_states.size();

    // Report the solvable states
    for (StackInfo* stkinfo : solvable_states) {
        const StateID sid = stkinfo->stateid;
        state_infos_[sid].solvable = true;
        *solvable_out = sid;
    }

    stack_.erase(scc.begin(), scc.end());
}

} // namespace preprocessing
} // namespace probfd
