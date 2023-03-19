#ifndef PROBFD_UTILS_POLICY_VERIFICATION_H
#define PROBFD_UTILS_POLICY_VERIFICATION_H

#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/state_space.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/policy.h"

#include <algorithm>
#include <cassert>
#include <deque>
#include <limits>
#include <unordered_set>

namespace probfd {

template <typename State, typename Action>
bool is_proper_policy(
    engine_interfaces::StateSpace<State, Action>& state_space,
    engine_interfaces::CostFunction<State, Action>& cost_function,
    PartialPolicy<State, Action>& policy,
    const State& source_state)
{
    static constexpr unsigned UNDEF = std::numeric_limits<unsigned>::max() >> 2;

    struct StateInfo {
        unsigned explored : 1 = 0;
        unsigned dead : 1 = 1;
        unsigned stackid : 30 = UNDEF;

        bool onstack() const { return stackid < UNDEF; }
    };

    struct ExpansionInfo {
        ExpansionInfo(unsigned stck, Distribution<StateID> transition)
            : stck(stck)
            , lstck(stck)
            , transition(std::move(transition))
            , successor(this->transition.begin())
        {
        }

        bool next_successor() { return ++successor != transition.end(); }

        StateID get_current_successor() { return successor->item; }

        // Tarjan's SCC algorithm: stack id and lowlink
        const unsigned stck;
        unsigned lstck;

        Distribution<StateID> transition;
        typename Distribution<StateID>::const_iterator successor;
    };

    storage::PerStateStorage<StateInfo> state_infos;
    std::deque<ExpansionInfo> expansion_queue;
    std::vector<StateID> stack;

    auto push = [&](StateID state_id, StateInfo& state_info) {
        assert(!state_info.explored && state_info.dead);

        state_info.explored = 1;

        State state = state_space.get_state(state_id);

        auto decision = policy.get_decision(state);

        if (cost_function.get_termination_info(state).is_goal_state()) {
            assert(!decision);
            state_info.dead = 0;
            return false;
        }

        if (!decision) {
            return false;
        }

        Distribution<StateID> transition;
        state_space.generate_action_transitions(
            state_id,
            decision->action,
            transition);

        // Check for self loop
        if (transition.is_dirac(state_id)) {
            return false;
        }

        const std::size_t stack_size = stack.size();
        state_info.stackid = stack_size;
        stack.emplace_back(state_id);
        expansion_queue.emplace_back(stack_size, std::move(transition));

        return true;
    };

    auto init_id = state_space.get_state_id(source_state);
    push(init_id, state_infos[init_id]);

    ExpansionInfo* e = &expansion_queue.back();
    StateID* stateid = &stack[e->stck];
    StateInfo* st = &state_infos[*stateid];

    for (;;) {
        // DFS recursion
        do {
            StateID succ_id = e->get_current_successor();
            StateInfo& succ_info = state_infos[succ_id];

            if (succ_info.onstack()) {
                e->lstck = std::min(e->lstck, succ_info.stackid);
            } else if (!succ_info.explored && push(succ_id, succ_info)) {
                e = &expansion_queue.back();
                stateid = &stack[e->stck];
                st = &state_infos[*stateid];
                goto continue_exploration;
            }

            st->dead = st->dead && succ_info.dead;
        } while (e->next_successor());

        // Repeated backtracking
        do {
            const unsigned stck = e->stck;
            const unsigned lstck = e->lstck;
            const bool onstack = stck != lstck;

            if (!onstack) {
                if (st->dead) {
                    return false;
                }

                stack.erase(stack.begin() + stck, stack.end());
            }

            expansion_queue.pop_back();

            if (expansion_queue.empty()) {
                goto break_exploration;
            }

            StateInfo& bt_info = state_infos[*stateid];
            StateID* backtracked_from = stateid;

            e = &expansion_queue.back();
            stateid = &stack[e->stck];
            st = &state_infos[*stateid];

            if (onstack) {
                e->lstck = std::min(e->lstck, lstck);
            }

            st->dead = st->dead && bt_info.dead;
        } while (!e->next_successor());

    continue_exploration:;
    }

break_exploration:;

    return true;
}

template <typename State, typename Action>
void verify_proper_policy(
    engine_interfaces::StateSpace<State, Action>& state_space,
    engine_interfaces::CostFunction<State, Action>& cost_function,
    PartialPolicy<State, Action>& policy,
    const State& source_state)
{
    if (!is_proper_policy(state_space, cost_function, policy, source_state)) {
        abort();
    }
}

} // namespace probfd

#endif