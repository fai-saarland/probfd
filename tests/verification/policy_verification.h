#ifndef PROBFD_TESTS_VERIFICATION_POLICY_VERIFICATION
#define PROBFD_TESTS_VERIFICATION_POLICY_VERIFICATION

#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/state_space.h"

#include "probfd/policy.h"

#include "probfd/storage/per_state_storage.h"

#include <ranges>
#include <stack>
#include <vector>

namespace probfd {
namespace tests {

/*
 * Verfies that a policy is proper for the initial state and that the state
 * values are correct by verifying the Bellman optimality equation.
 */
template <typename State, typename Action>
extern bool verify_policy(
    engine_interfaces::StateSpace<State, Action>& state_space,
    engine_interfaces::CostFunction<State, Action>& cost_function,
    PartialPolicy<State, Action>& policy,
    StateID init_id)
{
    struct StateInfo {
        bool is_dead = true;
        bool explored = false;
        unsigned stack_id = std::numeric_limits<unsigned>::max();
    };

    struct ExplorationInfo {
        ExplorationInfo(StateID state_id, unsigned stack_id)
            : state_id(state_id)
            , lowlink(stack_id)
        {
        }

        StateID state_id;
        unsigned lowlink = std::numeric_limits<unsigned>::max();

        Distribution<StateID> successors;
    };

    std::stack<ExplorationInfo> open;
    std::vector<StateID> stack;
    storage::PerStateStorage<StateInfo> state_infos;

    open.emplace(init_id, 0);

    for (;;) {
    recurse:;
        ExplorationInfo* info = &open.top();

        StateID state_id = info->state_id;
        State state = state_space.get_state(state_id);
        StateInfo* state_info = &state_infos[state_id];

        state_info->explored = true;
        state_info->stack_id = stack.size();
        stack.push_back(state_id);

        std::optional decision = policy.get_decision(state);

        // Check if goal. No decision in this case.
        if (cost_function.get_termination_info(state).is_goal_state()) {
            if (decision) return false;
            state_info->is_dead = false;
            goto backtracking;
        }

        // Otherwise, a decision must be made.
        if (!decision) return false;

        // Generate successors.
        state_space.generate_action_transitions(
            state_id,
            decision->action,
            info->successors);

        // Check Bellman equation
        {
            value_t expected_cost =
                cost_function.get_action_cost(decision->action);

            for (const auto [successor_id, probability] : info->successors) {
                const State successor = state_space.get_state(successor_id);
                std::optional succ_decision = policy.get_decision(successor);

                const value_t succ_val =
                    succ_decision ? succ_decision->q_value_interval.lower
                                  : 0_vt;

                expected_cost += probability * succ_val;
            }

            if (!is_approx_equal(
                    decision->q_value_interval.lower,
                    expected_cost))
                return false;
        }

        if (info->successors.empty()) abort();

        for (;;) {
            // DFS Expansion
            do {
                const StateID successor_id = (info->successors.end() - 1)->item;
                StateInfo& succ_info = state_infos[successor_id.id];

                if (!succ_info.explored) {
                    open.emplace(successor_id, stack.size());
                    goto recurse;
                }

                state_info->is_dead = state_info->is_dead && succ_info.is_dead;
                info->lowlink = std::min(info->lowlink, succ_info.stack_id);
                info->successors.erase(info->successors.end() - 1);
            } while (!info->successors.empty());

        backtracking:;

            // Backtracking
            do {
                const unsigned stack_id = state_info->stack_id;
                const unsigned lowlink = info->lowlink;

                // Check for SCC
                if (stack_id == lowlink) {
                    // SCC must be able to reach the goal.
                    if (state_info->is_dead) return false;

                    std::ranges::subrange scc(
                        stack.begin() + stack_id,
                        stack.end());

                    // Erase the scc from the stack.
                    for (const StateID state_id : scc) {
                        state_infos[state_id.id].stack_id =
                            std::numeric_limits<unsigned>::max();
                    }

                    stack.erase(scc.begin(), scc.end());
                }

                // Backtrack from successor
                open.pop();

                if (open.empty()) return true;

                info = &open.top();
                state_id = info->state_id;
                state = state_space.get_state(state_id);
                state_info = &state_infos[state_id];

                // The successor we backtracked from.
                const StateID successor_id = (info->successors.end() - 1)->item;

                const StateInfo& succ_info = state_infos[successor_id.id];
                state_info->is_dead = state_info->is_dead && succ_info.is_dead;
                info->lowlink = std::min(info->lowlink, lowlink);
                info->successors.erase(info->successors.end() - 1);
            } while (info->successors.empty());
        }
    }
}

} // namespace tests
} // namespace probfd

#endif