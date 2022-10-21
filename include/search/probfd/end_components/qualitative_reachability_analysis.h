#ifndef MDPS_END_COMPONENT_DECOMPOSITION_QUALITATIVE_REACHABILITY_ANALYSIS_H
#define MDPS_END_COMPONENT_DECOMPOSITION_QUALITATIVE_REACHABILITY_ANALYSIS_H

#include "utils/iterators.h"
#include "utils/timer.h"

#include "probfd/engine_interfaces/action_id_map.h"
#include "probfd/engine_interfaces/reward_function.h"
#include "probfd/engine_interfaces/state_evaluator.h"
#include "probfd/engine_interfaces/state_id_map.h"
#include "probfd/engine_interfaces/transition_generator.h"


#include "probfd/heuristics/pdbs/qualitative_result_store.h"

#include "probfd/quotient_system/engine_interfaces.h"
#include "probfd/quotient_system/quotient_system.h"

#include "probfd/storage/per_state_storage.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <stack>
#include <type_traits>
#include <vector>

namespace probfd {

namespace reachability {

struct Statistics {
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

    unsigned stackid() const
    {
        assert(onstack());
        return stackid_;
    }

    bool onstack() const { return stackid_ < UNDEF; }

    unsigned explored : 1;
    unsigned expandable_goal : 1; // non-terminal goal?
    unsigned dead : 1;            // dead end flag
    unsigned one : 1;             // proper state flag
    unsigned stackid_ : 28;
};

template <typename Action>
struct ExpansionInfo {
    explicit ExpansionInfo(unsigned stck)
        : stck(stck)
        , lstck(stck)
    {
    }

    // Tarjan's SCC algorithm: stack id and lowlink
    const unsigned stck;
    unsigned lstck;

    bool exits_only_proper = true;
    bool transitions_in_scc = false;

    std::vector<Action> aops;
    std::vector<std::vector<StateID>> successors;
};

struct StackInfo {
    explicit StackInfo(const StateID& sid)
        : stateid(sid)
    {
    }

    StateID stateid;

    unsigned scc_transitions = 0;

    std::vector<bool> active;
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
    using StateInfo = internal::StateInfo;
    using StateInfoStore = storage::PerStateStorage<StateInfo>;

    using ExpansionInfo = internal::ExpansionInfo<Action>;
    using ExpansionQueue = std::deque<ExpansionInfo>;

    using StackInfo = internal::StackInfo;
    using Stack = std::vector<StackInfo>;

public:
    QualitativeReachabilityAnalysis(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::RewardFunction<State, Action>* rewards,
        engine_interfaces::TransitionGenerator<Action>* transition_gen,
        bool expand_goals,
        const engine_interfaces::StateEvaluator<State>* pruning_function =
            nullptr)
        : state_id_map_(state_id_map)
        , action_id_map_(action_id_map)
        , rewards_(rewards)
        , transition_gen_(transition_gen)
        , expand_goals_(expand_goals)
        , pruning_function_(pruning_function)
    {
    }

    template <typename ZeroOutputIt, typename OneOutputIt>
    void run_analysis(
        const State& source_state,
        ZeroOutputIt zero_states_out,
        OneOutputIt one_states_out)
    {
        assert(expansion_queue_.empty());

        auto init_id = state_id_map_->get_state_id(source_state);
        push(init_id, state_infos_[init_id], zero_states_out, one_states_out);

        assert(!expansion_queue_.empty());

        ExpansionInfo* e = &expansion_queue_.back();
        StackInfo* s = &stack_[e->stck];
        StateInfo* st = &state_infos_[s->stateid];

        for (;;) {
            // DFS recursion
            while (
                push_successor(*e, *s, *st, zero_states_out, one_states_out)) {
                e = &expansion_queue_.back();
                s = &stack_[e->stck];
                st = &state_infos_[s->stateid];
            }

            StateInfo& bt_info = state_infos_[s->stateid];
            unsigned lstck = e->lstck;
            StackInfo* backtracked_from = s;
            bool onstack = e->stck != e->lstck;

            if (!onstack) {
                scc_found(
                    stack_.begin() + e->stck,
                    stack_.end(),
                    zero_states_out,
                    one_states_out);
            }

            expansion_queue_.pop_back();

            if (expansion_queue_.empty()) {
                break;
            }

            e = &expansion_queue_.back();
            s = &stack_[e->stck];
            st = &state_infos_[s->stateid];

            // We returned from a recursive DFS call. Update the parent.

            if (onstack) {
                e->lstck = std::min(e->lstck, lstck);
                e->transitions_in_scc = true;

                if (!expand_goals_ || !st->expandable_goal) {
                    auto& parents = backtracked_from->parents;
                    parents.emplace_back(e->stck, s->active.size());
                }
            } else {
                e->exits_only_proper = e->exits_only_proper && bt_info.one;
            }

            st->dead = st->dead && bt_info.dead;

            if (e->successors.back().empty()) {
                e->successors.pop_back();

                if (e->transitions_in_scc) {
                    if (e->exits_only_proper) ++s->scc_transitions;
                    s->active.push_back(e->exits_only_proper);
                } else if (e->exits_only_proper) {
                    st->one = true;
                }

                // Reset transition flags
                e->exits_only_proper = true;
                e->transitions_in_scc = false;
            }
        }
    }

private:
    template <typename ZeroOutputIt, typename OneOutputIt>
    bool push(
        const StateID& state_id,
        StateInfo& state_info,
        ZeroOutputIt zero_states_out,
        OneOutputIt one_states_out)
    {
        assert(!state_info.explored);
        assert(!state_info.one && state_info.dead);

        state_info.explored = 1;

        State state = state_id_map_->get_state(state_id);

        if (rewards_->operator()(state)) {
            ++stats_.terminals;
            ++stats_.goals;
            ++stats_.ones;

            state_info.dead = 0;
            state_info.one = 1;

            *one_states_out = state_id;

            if (!expand_goals_) {
                return false;
            }

            state_info.expandable_goal = 1;
        } else if (
            pruning_function_ != nullptr &&
            pruning_function_->operator()(state)) {
            ++stats_.terminals;
            *zero_states_out = state_id;
            return false;
        }

        std::vector<Action> aops;
        transition_gen_->operator()(state_id, aops);

        if (aops.empty()) {
            if (expand_goals_ && state_info.expandable_goal) {
                state_info.expandable_goal = 0;
            } else {
                ++stats_.terminals;
                *zero_states_out = state_id;
            }

            return false;
        }

        ExpansionInfo& e = expansion_queue_.emplace_back(stack_.size());

        e.successors.reserve(aops.size());

        for (const auto& action : aops) {
            Distribution<StateID> transition;
            transition_gen_->operator()(state_id, action, transition);

            std::vector<StateID> succs;

            for (const StateID& succ_id : transition.elements()) {
                if (succ_id != state_id) {
                    succs.push_back(succ_id);
                }
            }

            if (!succs.empty()) {
                e.successors.push_back(std::move(succs));
            }
        }

        state_info.stackid_ = stack_.size();
        stack_.emplace_back(state_id);

        return true;
    }

    template <typename ZeroOutputIt, typename OneOutputIt>
    bool push_successor(
        ExpansionInfo& e,
        StackInfo& s,
        StateInfo& st,
        ZeroOutputIt zero_states_out,
        OneOutputIt one_states_out)
    {
        while (!e.successors.empty()) {
            std::vector<StateID>& succs = e.successors.back();

            while (!succs.empty()) {
                StateID succ_id = succs.back();
                succs.pop_back();
                StateInfo& succ_info = state_infos_[succ_id];

                if (!succ_info.explored) {
                    if (push(
                            succ_id,
                            succ_info,
                            zero_states_out,
                            one_states_out)) {
                        return true;
                    }

                    assert(!succ_info.onstack());

                    if (!succ_info.one) {
                        e.exits_only_proper = false;
                    }

                    if (!succ_info.dead) {
                        st.dead = false;
                    }
                }

                if (succ_info.onstack()) {
                    unsigned succ_stack_id = succ_info.stackid_;
                    e.transitions_in_scc = true;
                    e.lstck = std::min(e.lstck, succ_stack_id);

                    if (!expand_goals_ || !st.expandable_goal) {
                        auto& parents = stack_[succ_stack_id].parents;
                        parents.emplace_back(e.stck, s.active.size());
                    }
                } else {
                    if (!succ_info.one) {
                        e.exits_only_proper = false;
                    }

                    if (!succ_info.dead) {
                        st.dead = false;
                    }
                }
            }

            assert(succs.empty());

            if (e.transitions_in_scc) {
                if (e.exits_only_proper) ++s.scc_transitions;
                s.active.push_back(e.exits_only_proper);
            } else if (e.exits_only_proper) {
                st.one = true;
            }

            // Reset transition flags
            e.exits_only_proper = true;
            e.transitions_in_scc = false;

            e.successors.pop_back();
        }

        return false;
    }

    template <typename ZeroOutputIt, typename OneOutputIt>
    void scc_found(
        decltype(std::declval<Stack>().begin()) begin,
        decltype(std::declval<Stack>().begin()) end,
        ZeroOutputIt zero_states_out,
        OneOutputIt one_states_out)
    {
        const StateInfo& st_info = state_infos_[begin->stateid];

        if (st_info.dead) {
            for (auto it = begin; it != end; ++it) {
                StateInfo& info = state_infos_[it->stateid];
                info.stackid_ = StateInfo::UNDEF;
                assert(info.dead);
                *zero_states_out = it->stateid;
            }
        } else {
            // Collect non-proper states
            std::deque<StackInfo*> non_one;
            {
                for (auto stk_it = begin; stk_it != end; ++stk_it) {
                    auto sid = stk_it->stateid;
                    StateInfo& sinfo = state_infos_[sid];

                    if (!sinfo.one && stk_it->scc_transitions == 0 &&
                        !stk_it->parents.empty()) {
                        non_one.push_back(&*stk_it);
                    }
                }
            }

            // Fix-point iteration to throw out actions which do not
            // uniformly lead to proper states. If no action remains the state
            // is not proper.
            while (!non_one.empty()) {
                StackInfo* scc_elem = non_one.back();
                non_one.pop_back();

                for (const auto& [first, second] : scc_elem->parents) {
                    StackInfo& pinfo = stack_[first];
                    const StateInfo& sinfo = state_infos_[pinfo.stateid];

                    if (!sinfo.one && pinfo.active[second]) {
                        pinfo.active[second] = false;
                        if (--pinfo.scc_transitions == 0 &&
                            !pinfo.parents.empty()) {
                            non_one.push_back(&pinfo);
                        }
                    }
                }
            }

            // Report the proper states
            for (auto stk_it = begin; stk_it != end; ++stk_it) {
                auto sid = stk_it->stateid;
                StateInfo& sinfo = state_infos_[sid];

                if (sinfo.one || stk_it->scc_transitions) {
                    if (!sinfo.expandable_goal) {
                        sinfo.one = true;
                        *one_states_out = sid;
                        ++stats_.ones;
                    }
                }

                sinfo.stackid_ = StateInfo::UNDEF;
            }
        }

        stack_.erase(begin, end);
    }

    engine_interfaces::StateIDMap<State>* state_id_map_;
    engine_interfaces::ActionIDMap<Action>* action_id_map_;
    engine_interfaces::RewardFunction<State, Action>* rewards_;
    engine_interfaces::TransitionGenerator<Action>* transition_gen_;

    bool expand_goals_;

    const engine_interfaces::StateEvaluator<State>* pruning_function_;

    StateInfoStore state_infos_;
    ExpansionQueue expansion_queue_;
    Stack stack_;

    Statistics stats_;
};

} // namespace reachability
} // namespace probfd

#endif // __END_COMPONENT_DECOMPOSITION_H__