#ifndef VERIFICATION_POLICY_VERIFICATION_H
#define VERIFICATION_POLICY_VERIFICATION_H

#include "probfd/storage/per_state_storage.h"

#include "probfd/mdp.h"
#include "probfd/policy.h"
#include "probfd/transition_tail.h"

#include <ranges>
#include <stack>
#include <vector>

namespace tests {

/*
 * Verfies that a policy is proper for the initial state and that the state
 * values are correct by verifying the Bellman optimality equation.
 */
template <typename State, typename Action>
extern bool verify_policy(
    probfd::MDP<State, Action>& mdp,
    probfd::Policy<State, Action>& policy,
    probfd::StateID init_id,
    probfd::value_t epsilon)
{
    using namespace probfd;

    struct StateInfo {
        bool is_dead = true;
        bool explored = false;
        unsigned stack_id = std::numeric_limits<unsigned>::max();
    };

    struct ExplorationInfo {
        ExplorationInfo(probfd::StateID state_id, unsigned stack_id)
            : state_id(state_id)
            , lowlink(stack_id)
        {
        }

        probfd::StateID state_id;
        unsigned lowlink = std::numeric_limits<unsigned>::max();

        SuccessorDistribution successors;
    };

    std::stack<ExplorationInfo> open;
    std::vector<probfd::StateID> stack;
    storage::PerStateStorage<StateInfo> state_infos;

    open.emplace(init_id, 0);

    for (;;) {
    recurse:;
        ExplorationInfo* info = &open.top();

        probfd::StateID state_id = info->state_id;
        State state = mdp.get_state(state_id);
        StateInfo* state_info = &state_infos[state_id];

        state_info->explored = true;
        state_info->stack_id = stack.size();
        stack.push_back(state_id);

        std::optional decision = policy.get_decision(state);

        // Check if goal. No decision in this case.
        if (mdp.get_termination_info(state).is_goal_state()) {
            if (decision) return false;
            state_info->is_dead = false;
            goto backtracking;
        }

        // Otherwise, a decision must be made.
        if (!decision) return false;

        // Generate successors.
        mdp.generate_action_transitions(
            state,
            decision->action,
            info->successors);

        // Check Bellman equation
        if (info->successors.non_source_successor_dist.empty()) abort();

        {
            value_t expected_cost = mdp.get_action_cost(decision->action);

            for (const auto [successor_id, probability] :
                 info->successors.non_source_successor_dist) {
                const State successor = mdp.get_state(successor_id);
                std::optional succ_decision = policy.get_decision(successor);

                const value_t succ_val =
                    succ_decision
                        ? succ_decision->q_value_interval.lower
                        : mdp.get_termination_info(successor).get_cost();

                expected_cost += probability * succ_val;
            }

            const auto value = decision->q_value_interval.lower;
            expected_cost +=
                (1 - info->successors.non_source_probability) * value;

            if (!is_approx_equal(value, expected_cost, epsilon)) return false;
        }

        for (;;) {
            // DFS Expansion
            do {
                const probfd::StateID successor_id =
                    std::prev(info->successors.non_source_successor_dist.end())
                        ->item;
                StateInfo& succ_info = state_infos[successor_id.id];

                if (!succ_info.explored) {
                    open.emplace(successor_id, stack.size());
                    goto recurse;
                }

                state_info->is_dead = state_info->is_dead && succ_info.is_dead;
                info->lowlink = std::min(info->lowlink, succ_info.stack_id);
                info->successors.non_source_successor_dist.erase(
                    std::prev(
                        info->successors.non_source_successor_dist.end()));
            } while (!info->successors.non_source_successor_dist.empty());

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
                    for (const probfd::StateID state_id : scc) {
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
                state = mdp.get_state(state_id);
                state_info = &state_infos[state_id];

                // The successor we backtracked from.
                const probfd::StateID successor_id =
                    (std::prev(
                         info->successors.non_source_successor_dist.end()))
                        ->item;

                const StateInfo& succ_info = state_infos[successor_id.id];
                state_info->is_dead = state_info->is_dead && succ_info.is_dead;
                info->lowlink = std::min(info->lowlink, lowlink);
                info->successors.non_source_successor_dist.erase(
                    info->successors.non_source_successor_dist.end() - 1);
            } while (info->successors.non_source_successor_dist.empty());
        }
    }
}

} // namespace tests

#endif // VERIFICATION_POLICY_VERIFICATION_H
