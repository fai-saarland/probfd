#ifndef MDPS_END_COMPONENT_DECOMPOSITION_QUALITATIVE_REACHABILITY_ANALYSIS_H
#define MDPS_END_COMPONENT_DECOMPOSITION_QUALITATIVE_REACHABILITY_ANALYSIS_H

#include "../../utils/iterators.h"
#include "../../utils/timer.h"
#include "../engine_interfaces/action_id_map.h"
#include "../engine_interfaces/reward_function.h"
#include "../engine_interfaces/state_evaluator.h"
#include "../engine_interfaces/state_id_map.h"
#include "../engine_interfaces/transition_generator.h"
#include "../heuristics/pdbs/qualitative_result_store.h"
#include "../quotient_system/engine_interfaces.h"
#include "../quotient_system/quotient_system.h"
#include "../storage/per_state_storage.h"


#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>

namespace probabilistic {

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
    static const unsigned UNDEF = (((unsigned)-1) >> 2) - 2;
    static const unsigned ZERO = UNDEF + 1;
    static const unsigned ONE = UNDEF + 2;

    explicit StateInfo()
        : explored(0)
        , expandable_goal(0)
        , stackid_(UNDEF)
    {
    }

    unsigned stackid() const
    {
        assert(onstack());
        return stackid_;
    }
    bool onstack() const { return stackid_ < UNDEF; }
    bool zero() const { return stackid_ == ZERO; }
    bool one() const { return stackid_ == ONE; }

    unsigned explored : 1;
    unsigned expandable_goal : 1; // non-terminal goal?
    unsigned stackid_ : 30;
};

template <typename Action>
struct ExpansionInfo {
    explicit ExpansionInfo(unsigned stck, bool non_expandable_goal)
        : stck(stck)
        , lstck(stck)
        , non_expandable_goal(non_expandable_goal)
    {
    }

    // Tarjan's SCC algorithm: stack id and lowlink
    const unsigned stck;
    unsigned lstck;

    const bool non_expandable_goal;

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

    bool dead = true;
    bool one = false;
    unsigned scc_transitions = 0;

    std::vector<bool> active;
    std::vector<std::pair<unsigned, unsigned>> parents;
};

} // namespace internal

/**
 * @brief Algorithm that computes all dead-ends and states with a goal
 * probability of one.
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
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        RewardFunction<State, Action>* rewards,
        TransitionGenerator<Action>* transition_gen,
        bool expand_goals,
        const StateEvaluator<State>* pruning_function = nullptr)
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

        for (;;) {
            // DFS recursion
            while (push_successor(*e, *s, zero_states_out, one_states_out)) {
                e = &expansion_queue_.back();
                s = &stack_[e->stck];
            }

            bool dead = s->dead;
            bool one = s->one;
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

            // We returned from a recursive DFS call. Update the parent.

            if (onstack) {
                e->lstck = std::min(e->lstck, lstck);
                e->transitions_in_scc = true;

                if (!expand_goals_ || e->non_expandable_goal) {
                    auto& parents = backtracked_from->parents;
                    parents.emplace_back(e->stck, s->active.size());
                }
            } else {
                e->exits_only_proper = e->exits_only_proper && one;
            }

            s->dead = s->dead && dead;

            if (e->successors.back().empty()) {
                e->successors.pop_back();

                if (e->transitions_in_scc) {
                    if (e->exits_only_proper) ++s->scc_transitions;
                    s->active.push_back(e->exits_only_proper);
                } else if (e->exits_only_proper) {
                    s->one = true;
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
        assert(!state_info.one() && !state_info.zero());

        state_info.explored = 1;

        State state = state_id_map_->get_state(state_id);

        if (rewards_->operator()(state)) {
            ++stats_.terminals;
            ++stats_.goals;
            ++stats_.ones;
            *one_states_out = state_id;
            state_info.stackid_ = StateInfo::ONE;

            if (!expand_goals_) {
                return false;
            }

            state_info.expandable_goal = 1;
        } else if (
            pruning_function_ != nullptr &&
            pruning_function_->operator()(state)) {
            ++stats_.terminals;
            *zero_states_out = state_id;
            state_info.stackid_ = StateInfo::ZERO;
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
                state_info.stackid_ = StateInfo::ZERO;
            }

            return false;
        }

        ExpansionInfo& e = expansion_queue_.emplace_back(
            stack_.size(),
            !state_info.expandable_goal);

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

        StackInfo& info = stack_.emplace_back(state_id);
        info.dead = !state_info.expandable_goal;
        info.one = expand_goals_ && state_info.expandable_goal;

        return true;
    }

    template <typename ZeroOutputIt, typename OneOutputIt>
    bool push_successor(
        ExpansionInfo& e,
        StackInfo& s,
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

                    if (!succ_info.one()) {
                        e.exits_only_proper = false;
                    }

                    if (!succ_info.zero()) {
                        s.dead = false;
                    }
                }

                if (succ_info.onstack()) {
                    unsigned succ_stack_id = succ_info.stackid_;
                    e.transitions_in_scc = true;
                    e.lstck = std::min(e.lstck, succ_stack_id);

                    if (!expand_goals_ || e.non_expandable_goal) {
                        auto& parents = stack_[succ_stack_id].parents;
                        parents.emplace_back(e.stck, s.active.size());
                    }
                } else {
                    if (!succ_info.one()) {
                        e.exits_only_proper = false;
                    }

                    if (!succ_info.zero()) {
                        s.dead = false;
                    }
                }
            }

            assert(succs.empty());

            if (e.transitions_in_scc) {
                if (e.exits_only_proper) ++s.scc_transitions;
                s.active.push_back(e.exits_only_proper);
            } else if (e.exits_only_proper) {
                s.one = true;
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
        if (begin->dead) {
            for (auto it = begin; it != end; ++it) {
                StateInfo& info = state_infos_[it->stateid];
                info.stackid_ = StateInfo::ZERO;
                *zero_states_out = it->stateid;
            }
        } else {
            // Collect non-proper states
            std::deque<StackInfo*> non_one;
            {
                for (auto stk_it = begin; stk_it != end; ++stk_it) {
                    if (!stk_it->one && stk_it->scc_transitions == 0 &&
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
                    if (!pinfo.one && pinfo.active[second]) {
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

                if (stk_it->one || stk_it->scc_transitions) {
                    sinfo.stackid_ = StateInfo::ONE;

                    if (!sinfo.expandable_goal) {
                        *one_states_out = sid;
                    }

                    ++stats_.ones;
                } else {
                    sinfo.stackid_ = StateInfo::UNDEF;
                }
            }
        }

        stack_.erase(begin, end);
    }

    StateIDMap<State>* state_id_map_;
    ActionIDMap<Action>* action_id_map_;
    RewardFunction<State, Action>* rewards_;
    TransitionGenerator<Action>* transition_gen_;

    bool expand_goals_;

    const StateEvaluator<State>* pruning_function_;

    StateInfoStore state_infos_;
    ExpansionQueue expansion_queue_;
    Stack stack_;

    Statistics stats_;
};

} // namespace reachability
} // namespace probabilistic

#endif // __END_COMPONENT_DECOMPOSITION_H__