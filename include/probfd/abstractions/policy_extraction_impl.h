#ifndef GUARD_INCLUDE_PROBFD_ABSTRACTIONS_POLICY_EXTRACTION_H
#error "This file should only be included from policy_extraction.h"
#endif

#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/policies/vector_multi_policy.h"

#include "probfd/distribution.h"
#include "probfd/transition_tail.h"

#include "downward/utils/rng.h"

#include <cassert>
#include <deque>
#include <unordered_set>
#include <utility>
#include <vector>

namespace probfd {

template <typename State, typename Action>
std::unique_ptr<MultiPolicy<State, Action>> compute_optimal_projection_policy(
    MDP<State, Action>& mdp,
    std::span<const value_t> value_table,
    ParamType<State> initial_state,
    value_t greedy_epsilon,
    utils::RandomNumberGenerator& rng,
    bool wildcard)
{
    using PredecessorEdge = std::pair<State, Action>;

    std::deque<StateID> open = {initial_state};
    std::unordered_set<StateID> closed = {initial_state};

    std::vector<std::vector<PredecessorEdge>> predecessors(value_table.size());
    std::vector<StateID> goals;

    // Build the greedy policy graph
    while (!open.empty()) {
        StateID s = open.front();
        open.pop_front();

        const State state = mdp.get_state(s);

        const value_t value = value_table[s];

        // Skip states in which termination is optimal
        const value_t term_cost = mdp.get_termination_info(s).get_cost();
        if (value == term_cost) {
            goals.push_back(s);
            continue;
        }

        // Generate operators...
        std::vector<TransitionTail<Action>> transitions;
        mdp.generate_all_transitions(state, transitions);

        // Select the greedy operators and add their successors
        for (const auto& [op, successor_dist] : transitions) {
            value_t op_value = mdp.get_action_cost(op) +
                               successor_dist.expectation(value_table);

            if (!is_approx_equal(value, op_value, greedy_epsilon)) continue;

            for (const StateID succ : successor_dist.support()) {
                if (closed.insert(succ).second) {
                    open.push_back(succ);
                }

                predecessors[succ].emplace_back(s, op);
            }
        }
    }

    auto policy = std::make_unique<policies::VectorMultiPolicy<State, Action>>(
        &mdp,
        value_table.size());

    // Do regression search with duplicate checking through the constructed
    // graph, expanding predecessors randomly to select an optimal policy
    assert(open.empty());
    open.insert(open.end(), goals.begin(), goals.end());
    closed.clear();
    closed.insert(goals.begin(), goals.end());

    while (!open.empty()) {
        // Choose a random successor
        auto it = rng.choose(open);
        StateID s = *it;

        std::swap(*it, open.back());
        open.pop_back();

        // Consider predecessors in random order
        rng.shuffle(predecessors[s]);

        for (const auto& [pstate_id, sel_op] : predecessors[s]) {
            if (!closed.insert(pstate_id).second) continue;
            open.push_back(pstate_id);

            const value_t parent_cost = value_table[pstate_id];

            const State pstate = mdp.get_state(pstate_id);

            // Collect all equivalent greedy operators
            const value_t cost_sel_op = mdp.get_action_cost(sel_op);
            Distribution<StateID> sel_successor_dist;
            mdp.generate_action_transitions(pstate, sel_op, sel_successor_dist);

            std::vector<PolicyDecision<Action>> decisions;

            std::vector<TransitionTail<Action>> transitions;
            mdp.generate_all_transitions(pstate, transitions);

            for (const auto& [op, successor_dist] : transitions) {
                if (successor_dist == sel_successor_dist &&
                    mdp.get_action_cost(op) == cost_sel_op) {
                    decisions.emplace_back(op, Interval(parent_cost));
                }
            }

            // If not wildcard, randomly pick one
            if (!wildcard) decisions = {*rng.choose(decisions)};

            (*policy)[pstate_id] = std::move(decisions);

            assert(!(*policy)[pstate_id].empty());
        }
    }

    return policy;
}

template <typename State, typename Action>
std::unique_ptr<MultiPolicy<State, Action>> compute_greedy_projection_policy(
    MDP<State, Action>& mdp,
    std::span<const value_t> value_table,
    ParamType<State> initial_state,
    value_t greedy_epsilon,
    utils::RandomNumberGenerator& rng,
    bool wildcard)
{
    auto policy = std::make_unique<policies::VectorMultiPolicy<State, Action>>(
        &mdp,
        value_table.size());

    std::deque<StateID> open{initial_state};
    std::unordered_set<StateID> closed{initial_state};

    // Build the greedy policy graph
    while (!open.empty()) {
        StateID s = open.front();
        open.pop_front();

        const value_t value = value_table[s];

        const State state = mdp.get_state(s);

        // Skip states in which termination is optimal
        const value_t term_cost = mdp.get_termination_info(state).get_cost();
        if (value == term_cost) {
            continue;
        }

        // Generate operators...
        std::vector<TransitionTail<Action>> transitions;
        mdp.generate_all_transitions(state, transitions);

        if (transitions.empty()) {
            continue;
        }

        // Look at the (greedy) operators in random order.
        rng.shuffle(transitions);

        // Find first greedy transition
        auto it = std::ranges::find(transitions, [&](const auto& transition) {
            const auto& [op, successor_dist] = transition;
            const value_t op_value = mdp.get_action_cost(op) +
                                     successor_dist.expectation(value_table);

            return is_approx_equal(value, op_value, greedy_epsilon);
        });

        assert(it != transitions.end());

        const value_t cost_greedy = mdp.get_action_cost(it->action);

        // Generate successors
        for (const StateID succ : it->successor_dist.support()) {
            if (!closed.insert(succ).second) continue;
            open.push_back(succ);
        }

        // Collect all equivalent greedy operators
        std::vector<PolicyDecision<Action>> decisions;
        decisions.emplace_back(it->action, Interval(value));

        for (const auto& [op, successor_dist] :
             std::ranges::subrange(std::next(it), transitions.end())) {
            const value_t cost_op = mdp.get_action_cost(op);

            if (successor_dist == it->successor_dist &&
                cost_op == cost_greedy) {
                decisions.emplace_back(op, Interval(value));
            }
        }

        // If not wildcard, randomly pick one
        if (!wildcard) decisions = {*rng.choose(decisions)};
        (*policy)[s] = std::move(decisions);

        assert(!(*policy)[s].empty());
    }

    return policy;
}

} // namespace probfd