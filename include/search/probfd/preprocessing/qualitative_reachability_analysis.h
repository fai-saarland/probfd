#ifndef PROBFD_PREPROCESSING_QUALITATIVE_REACHABILITY_ANALYSIS_H
#define PROBFD_PREPROCESSING_QUALITATIVE_REACHABILITY_ANALYSIS_H

#include "probfd/engine_interfaces/evaluator.h"
#include "probfd/engine_interfaces/mdp.h"

#include "probfd/quotients/engine_interfaces.h"
#include "probfd/quotients/quotient_system.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/utils/iterators.h"

#include "probfd/type_traits.h"

#include "utils/countdown_timer.h"
#include "utils/timer.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <limits>
#include <type_traits>
#include <vector>

namespace probfd {

namespace preprocessing {

struct QRStatistics {
    void print(std::ostream& out) const
    {
        out << "  Terminal states: " << terminals << " (" << goals
            << " goal states, " << selfloops << " self loop states)"
            << std::endl;
        out << "  Singleton SCC(s): " << sccs1 << " (" << sccs1_dead << " dead)"
            << std::endl;
        out << "  Non-singleton SCC(s): " << sccsk << " (" << sccsk_dead
            << " dead)" << std::endl;
        out << "  Qualitative reachability analysis: " << time << std::endl;
        out << "  " << ones << " state(s) can reach the goal with certainty"
            << std::endl;
    }

    unsigned long long goals = 0;
    unsigned long long terminals = 0;
    unsigned long long selfloops = 0;

    unsigned long long sccs1 = 0;
    unsigned long long sccsk = 0;
    unsigned long long sccs1_dead = 0;
    unsigned long long sccsk_dead = 0;
    unsigned long long ones = 0;

    utils::Timer time;
};

namespace internal {

struct StateInfo {
    static const unsigned UNDEF = std::numeric_limits<unsigned>::max() >> 4;

    explicit StateInfo()
        : explored(0)
        , expandable_goal(0)
        , dead(1)
        , one(0)
        , stackid_(UNDEF)
    {
    }

    bool onstack() const { return stackid_ < UNDEF; }

    unsigned explored : 1;
    unsigned expandable_goal : 1; // non-terminal goal?
    unsigned dead : 1;            // dead end flag
    unsigned one : 1;             // proper state flag
    unsigned stackid_ : 28;
};

struct StackInfo {
    explicit StackInfo(StateID sid)
        : stateid(sid)
    {
    }

    StateID stateid;

    unsigned proper_transitions = 0;
    unsigned exit_transitions = 0;

    std::vector<bool> active;
    std::vector<bool> exiting;
    std::vector<std::pair<unsigned, unsigned>> parents;
};

} // namespace internal

/**
 * @brief Algorithm that computes all dead-ends and states with a goal
 * probability of one which are reachable from a given initial state.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class QualitativeReachabilityAnalysis {
    struct ExpansionInfo {
        explicit ExpansionInfo(
            unsigned stck,
            std::vector<Action> aops,
            Distribution<StateID> transition)
            : stck(stck)
            , lstck(stck)
            , aops(std::move(aops))
            , transition(std::move(transition))
            , successor(this->transition.begin())
        {
        }

        /**
         * Advances to the next non-loop action. Returns nullptr if such an
         * action does not exist.
         */
        bool next_action(
            engine_interfaces::MDP<State, Action>& mdp,
            StateID state_id)
        {
            for (aops.pop_back(); !aops.empty(); aops.pop_back()) {
                transition.clear();
                mdp.generate_action_transitions(
                    state_id,
                    aops.back(),
                    transition);

                if (!transition.is_dirac(state_id)) {
                    successor = transition.begin();

                    // Reset transition flags
                    exits_only_proper = true;
                    transitions_in_scc = false;
                    exits_scc = false;

                    return true;
                }
            }

            return false;
        }

        bool next_successor() { return ++successor != transition.end(); }

        StateID get_current_successor() { return successor->item; }

        // Tarjan's SCC algorithm: stack id and lowlink
        const unsigned stck;
        unsigned lstck;

        bool exits_only_proper = true;
        bool transitions_in_scc = false;
        bool exits_scc = false;

        // Mutable info
        std::vector<Action> aops;         // Remaining unexpanded operators
        Distribution<StateID> transition; // Currently expanded transition
        // Next state to expand
        typename Distribution<StateID>::const_iterator successor;
    };

    using StateInfo = internal::StateInfo;
    using StackInfo = internal::StackInfo;

    bool expand_goals_;
    const engine_interfaces::Evaluator<State>* pruning_function_;

    storage::PerStateStorage<StateInfo> state_infos_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::vector<StackInfo> stack_;

    QRStatistics stats_;

public:
    QualitativeReachabilityAnalysis(
        bool expand_goals,
        const engine_interfaces::Evaluator<State>* pruning_function = nullptr)
        : expand_goals_(expand_goals)
        , pruning_function_(pruning_function)
    {
    }

    void run_analysis(
        engine_interfaces::MDP<State, Action> mdp,
        param_type<State> source_state,
        std::output_iterator<StateID> auto dead_out,
        std::output_iterator<StateID> auto non_proper_out,
        std::output_iterator<StateID> auto proper_out,
        double max_time = std::numeric_limits<double>::infinity())
    {
        assert(expansion_queue_.empty());

        utils::CountdownTimer timer(max_time);

        auto init_id = mdp.get_state_id(source_state);
        if (!push(
                mdp,
                init_id,
                state_infos_[init_id],
                dead_out,
                non_proper_out,
                proper_out)) {
            return;
        }

        assert(!expansion_queue_.empty());

        ExpansionInfo* e = &expansion_queue_.back();
        StackInfo* s = &stack_[e->stck];
        StateInfo* st = &state_infos_[s->stateid];

        for (;;) {
            // DFS recursion
            while (push_successor(
                mdp,
                *e,
                *s,
                *st,
                dead_out,
                non_proper_out,
                proper_out,
                timer)) {
                e = &expansion_queue_.back();
                s = &stack_[e->stck];
                st = &state_infos_[s->stateid];
            }

            // Repeated backtracking
            do {
                const unsigned stck = e->stck;
                const unsigned lstck = e->lstck;
                const bool onstack = stck != lstck;

                if (!onstack) {
                    scc_found(
                        stack_.begin() + stck,
                        stack_.end(),
                        dead_out,
                        non_proper_out,
                        proper_out,
                        timer);
                }

                expansion_queue_.pop_back();

                if (expansion_queue_.empty()) {
                    goto break_exploration;
                }

                timer.throw_if_expired();

                StateInfo& bt_info = state_infos_[s->stateid];
                StackInfo* backtracked_from = s;

                e = &expansion_queue_.back();
                s = &stack_[e->stck];
                st = &state_infos_[s->stateid];

                // Finalize explored transition.
                if (onstack) {
                    e->lstck = std::min(e->lstck, lstck);
                    e->transitions_in_scc = true;

                    if (!st->expandable_goal) {
                        auto& parents = backtracked_from->parents;
                        parents.emplace_back(e->stck, s->active.size());
                    }
                } else {
                    e->exits_only_proper = e->exits_only_proper && bt_info.one;
                    e->exits_scc = true;
                }

                st->dead = st->dead && bt_info.dead;

                // If a successor exists stop backtracking
                if (e->next_successor()) {
                    break;
                }

                // Finalize fully explored action successors.
                if (e->transitions_in_scc) {
                    if (e->exits_only_proper) {
                        if (e->exits_scc) {
                            ++s->exit_transitions;
                        }
                        ++s->proper_transitions;
                    }
                    s->active.push_back(e->exits_only_proper);
                    s->exiting.push_back(e->exits_only_proper && e->exits_scc);
                } else if (e->exits_only_proper) {
                    st->one = true;
                }
            } while (!e->next_action(mdp, s->stateid));
        }

    break_exploration:;
    }

private:
    bool push(
        engine_interfaces::MDP<State, Action> mdp,
        StateID state_id,
        StateInfo& state_info,
        std::output_iterator<StateID> auto dead_out,
        std::output_iterator<StateID> auto non_proper_out,
        std::output_iterator<StateID> auto proper_out)
    {
        assert(!state_info.explored);
        assert(!state_info.one && state_info.dead);

        state_info.explored = 1;

        State state = mdp.get_state(state_id);

        if (mdp.get_termination_info(state).is_goal_state()) {
            ++stats_.goals;

            state_info.dead = 0;
            state_info.one = 1;

            if (!expand_goals_) {
                ++stats_.terminals;
                *proper_out = state_id;
                return false;
            }

            state_info.expandable_goal = 1;
        } else if (
            pruning_function_ != nullptr &&
            pruning_function_->evaluate(state).is_unsolvable()) {
            ++stats_.terminals;
            *dead_out = state_id;
            *non_proper_out = state_id;
            return false;
        }

        std::vector<Action> aops;
        mdp.generate_applicable_actions(state_id, aops);

        if (aops.empty()) {
            ++stats_.terminals;

            if (state_info.expandable_goal) {
                state_info.expandable_goal = 0;
            } else {
                *dead_out = state_id;
                *non_proper_out = state_id;
            }

            return false;
        }

        /************** Forward to first non self loop ****************/
        Distribution<StateID> transition;

        do {
            mdp.generate_action_transitions(state_id, aops.back(), transition);

            assert(!transition.empty());

            // Check for self loop
            if (!transition.is_dirac(state_id)) {
                const std::size_t stack_size = stack_.size();

                state_info.stackid_ = stack_size;

                stack_.emplace_back(state_id);

                expansion_queue_.emplace_back(
                    stack_size,
                    std::move(aops),
                    std::move(transition));

                return true;
            }

            transition.clear();
            aops.pop_back();
        } while (!aops.empty());
        /*****************************************************************/

        if (state_info.expandable_goal) {
            state_info.expandable_goal = 0;
        } else {
            *dead_out = state_id;
            *non_proper_out = state_id;
        }

        return false;
    }

    bool push_successor(
        engine_interfaces::MDP<State, Action> mdp,
        ExpansionInfo& e,
        StackInfo& s,
        StateInfo& st,
        std::output_iterator<StateID> auto dead_out,
        std::output_iterator<StateID> auto non_proper_out,
        std::output_iterator<StateID> auto proper_out,
        utils::CountdownTimer& timer)
    {
        do {
            do {
                timer.throw_if_expired();

                StateID succ_id = e.get_current_successor();
                StateInfo& succ_info = state_infos_[succ_id];

                if (succ_info.onstack()) {
                    unsigned succ_stack_id = succ_info.stackid_;
                    e.lstck = std::min(e.lstck, succ_stack_id);

                    e.transitions_in_scc = true;

                    if (!st.expandable_goal) {
                        auto& parents = stack_[succ_stack_id].parents;
                        parents.emplace_back(e.stck, s.active.size());
                    }
                } else if (
                    !succ_info.explored && push(
                                               mdp,
                                               succ_id,
                                               succ_info,
                                               dead_out,
                                               non_proper_out,
                                               proper_out)) {
                    return true;
                } else {
                    e.exits_only_proper = e.exits_only_proper && succ_info.one;
                    e.exits_scc = true;
                }

                st.dead = st.dead && succ_info.dead;
            } while (e.next_successor());

            if (e.transitions_in_scc) {
                if (e.exits_only_proper) {
                    if (e.exits_scc) {
                        ++s.exit_transitions;
                    }
                    ++s.proper_transitions;
                }
                s.active.push_back(e.exits_only_proper);
                s.exiting.push_back(e.exits_only_proper && e.exits_scc);
            } else if (e.exits_only_proper) {
                st.one = true;
            }
        } while (e.next_action(mdp, s.stateid));

        return false;
    }

    void scc_found(
        std::forward_iterator auto begin,
        std::forward_iterator auto end,
        std::output_iterator<StateID> auto dead_out,
        std::output_iterator<StateID> auto non_proper_out,
        std::output_iterator<StateID> auto proper_out,
        utils::CountdownTimer& timer)
    {
        const StateInfo& st_info = state_infos_[begin->stateid];

        if (st_info.dead) {
            for (auto it = begin; it != end; ++it) {
                StateInfo& info = state_infos_[it->stateid];
                info.stackid_ = StateInfo::UNDEF;
                assert(info.dead);
                *dead_out = it->stateid;
                *non_proper_out = it->stateid;
            }
        } else {
            std::set<StackInfo*> proper_states;
            std::set<StackInfo*> exits;

            for (auto stk_it = begin; stk_it != end; ++stk_it) {
                proper_states.insert(&*stk_it);

                StateInfo& state_info = state_infos_[stk_it->stateid];
                state_info.stackid_ = StateInfo::UNDEF;
                state_info.dead = false;

                if (state_info.expandable_goal) {
                    assert(state_info.one);
                    *proper_out = stk_it->stateid;
                    ++stats_.ones;
                }

                if (stk_it->exit_transitions > 0 || state_info.one) {
                    exits.insert(&*stk_it);
                }
            }

            std::size_t prev_size;
            std::size_t current_size = proper_states.size();

            // Compute the set of proper states by iterative pruning of certain
            // states and transitions from a sub-SSP of the SCC. Initially,
            // this sub-SSP consists of all states and transitions except those
            // transitions which can lead to a non-proper state in a child SCC.
            do {
                timer.throw_if_expired();

                // Collect states that can currently reach a proper scc exit
                // or a goal state within the sub-SSP.
                std::set<StackInfo*> can_reach_exits(
                    exits.begin(),
                    exits.end());

                {
                    std::vector<StackInfo*> queue(exits.begin(), exits.end());

                    while (!queue.empty()) {
                        StackInfo* scc_elem = queue.back();
                        queue.pop_back();

                        for (const auto& [first, second] : scc_elem->parents) {
                            StackInfo& pinfo = stack_[first];

                            if (pinfo.active[second] &&
                                can_reach_exits.insert(&pinfo).second) {
                                queue.push_back(&pinfo);
                            }
                        }
                    }
                }

                // The complement is improper.
                std::set<StackInfo*> proven_improper;
                std::set_difference(
                    proper_states.begin(),
                    proper_states.end(),
                    can_reach_exits.begin(),
                    can_reach_exits.end(),
                    std::inserter(proven_improper, proven_improper.end()));

                // Iteratively prune improper states and transitions which
                // have these states as a target. If no transition remains for
                // a former parent in the process, the state becomes improper
                // as well and is added to the queue. Do this until nothing
                // changes anymore.
                std::vector<StackInfo*> queue(
                    proven_improper.begin(),
                    proven_improper.end());

                while (!queue.empty()) {
                    timer.throw_if_expired();

                    StackInfo* scc_elem = queue.back();
                    queue.pop_back();

                    // The state is not proper
                    assert(proper_states.find(scc_elem) != proper_states.end());
                    proper_states.erase(scc_elem);

                    *non_proper_out = scc_elem->stateid;

                    for (const auto& [first, second] : scc_elem->parents) {
                        StackInfo& pinfo = stack_[first];
                        const StateInfo& sinfo = state_infos_[pinfo.stateid];

                        if (sinfo.one || proven_improper.contains(&pinfo)) {
                            continue;
                        }

                        if (pinfo.exiting[second]) {
                            assert(pinfo.active[second]);
                            pinfo.active[second] = false;
                            pinfo.exiting[second] = false;

                            if (--pinfo.proper_transitions == 0) {
                                assert(!proven_improper.contains(&pinfo));
                                queue.push_back(&pinfo);
                            }

                            if (--pinfo.exit_transitions == 0) {
                                exits.erase(&pinfo);
                            }
                        } else if (pinfo.active[second]) {
                            pinfo.active[second] = false;

                            if (--pinfo.proper_transitions == 0) {
                                assert(!proven_improper.contains(&pinfo));
                                queue.push_back(&pinfo);
                            }
                        }
                    }
                }

                prev_size = current_size;
                current_size = proper_states.size();
            } while (prev_size != current_size);

            // Report the proper states
            for (StackInfo* stkinfo : proper_states) {
                auto sid = stkinfo->stateid;
                auto& sinfo = state_infos_[sid];

                if (!sinfo.expandable_goal) {
                    sinfo.one = true;
                    *proper_out = sid;
                    ++stats_.ones;
                }
            }
        }

        stack_.erase(begin, end);
    }
};

} // namespace preprocessing
} // namespace probfd

#endif // __END_COMPONENT_DECOMPOSITION_H__