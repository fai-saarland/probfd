#include "probfd/heuristics/pdbs/policy_extraction.h"

#include "probfd/heuristics/pdbs/projection_state_space.h"

#include "probfd/policies/vector_multi_policy.h"

#include "downward/utils/rng.h"

#include <deque>
#include <unordered_set>

namespace probfd::heuristics::pdbs {

std::unique_ptr<ProjectionMultiPolicy> compute_optimal_projection_policy(
    ProjectionStateSpace& mdp,
    std::span<const value_t> value_table,
    StateRank initial_state,
    utils::RandomNumberGenerator& rng,
    bool wildcard)
{
    using PredecessorList =
        std::vector<std::pair<StateRank, const ProjectionOperator*>>;

    const value_t term_cost = mdp.get_non_goal_termination_cost();

    assert(value_table[initial_state] != term_cost);

    std::unique_ptr policy = std::make_unique<
        policies::VectorMultiPolicy<StateRank, const ProjectionOperator*>>(
        &mdp,
        value_table.size());

    // return empty policy indicating unsolvable
    if (mdp.is_goal(initial_state)) {
        return policy;
    }

    std::map<StateRank, PredecessorList> predecessors;

    std::deque<StateRank> open;
    std::unordered_set<StateRank> closed;
    open.push_back(initial_state);
    closed.insert(initial_state);

    std::vector<StateRank> goals;

    // Build the greedy policy graph
    while (!open.empty()) {
        StateRank s = open.front();
        open.pop_front();

        const value_t value = value_table[s];

        // Skip dead-ends, the operator is irrelevant
        if (value == term_cost) {
            continue;
        }

        // Generate operators...
        std::vector<const ProjectionOperator*> aops;
        mdp.generate_applicable_actions(s, aops);

        // Select the greedy operators and add their successors
        for (const ProjectionOperator* op : aops) {
            value_t op_value = mdp.get_action_cost(op);

            std::vector<StateRank> successors;

            Distribution<StateID> successor_dist;
            mdp.generate_action_transitions(s, op, successor_dist);

            for (const auto& [t, prob] : successor_dist) {
                op_value += prob * value_table[t.id];
                successors.push_back(StateRank(t));
            }

            if (is_approx_equal(value, op_value)) {
                for (const StateRank succ : successors) {
                    if (mdp.is_goal(succ)) {
                        goals.push_back(succ);
                    } else if (closed.insert(succ).second) {
                        open.push_back(succ);
                        predecessors[succ] = PredecessorList();
                    }

                    predecessors[succ].emplace_back(s, op);
                }
            }
        }
    }

    // Do regression search with duplicate checking through the constructed
    // graph, expanding predecessors randomly to select an optimal policy
    assert(open.empty());
    open.insert(open.end(), goals.begin(), goals.end());
    closed.clear();
    closed.insert(goals.begin(), goals.end());

    while (!open.empty()) {
        // Choose a random successor
        auto it = rng.choose(open);
        StateRank s = *it;

        std::swap(*it, open.back());
        open.pop_back();

        // Consider predecessors in random order
        rng.shuffle(predecessors[s]);

        for (const auto& [pstate, sel_op] : predecessors[s]) {
            if (!closed.insert(pstate).second) continue;
            open.push_back(pstate);

            const value_t parent_cost = value_table[pstate];

            // Collect all equivalent greedy operators
            std::vector<const ProjectionOperator*> aops;
            mdp.generate_applicable_actions(pstate, aops);

            const value_t cost_sel_op = mdp.get_action_cost(sel_op);

            std::vector<PolicyDecision<const ProjectionOperator*>> decisions;

            for (const ProjectionOperator* op : aops) {
                const value_t cost_op = mdp.get_action_cost(op);
                if (are_equivalent(*op, *sel_op) && cost_op == cost_sel_op) {
                    decisions.emplace_back(op, Interval(parent_cost));
                }
            }

            // If not wildcard, randomly pick one
            if (!wildcard) decisions = {*rng.choose(decisions)};

            (*policy)[pstate] = std::move(decisions);
        }
    }

    return policy;
}

std::unique_ptr<ProjectionMultiPolicy> compute_greedy_projection_policy(
    ProjectionStateSpace& mdp,
    std::span<const value_t> value_table,
    StateRank initial_state,
    utils::RandomNumberGenerator& rng,
    bool wildcard)
{
    const value_t term_cost = mdp.get_non_goal_termination_cost();

    std::unique_ptr policy = std::make_unique<
        policies::VectorMultiPolicy<StateRank, const ProjectionOperator*>>(
        &mdp,
        value_table.size());

    if (mdp.is_goal(initial_state)) {
        return policy;
    }

    std::deque<StateRank> open;
    std::unordered_set<StateRank> closed;
    open.push_back(initial_state);
    closed.insert(initial_state);

    // Build the greedy policy graph
    while (!open.empty()) {
        StateRank s = open.front();
        open.pop_front();

        const value_t value = value_table[s];

        // Skip dead-ends, the operator is irrelevant
        if (value == term_cost) {
            continue;
        }

        // Generate operators...
        std::vector<const ProjectionOperator*> aops;
        mdp.generate_applicable_actions(s, aops);

        if (aops.empty()) {
            continue;
        }

        // Look at the (greedy) operators in random order.
        rng.shuffle(aops);

        const ProjectionOperator* greedy;
        std::vector<StateRank> greedy_successors;

        // Select first greedy operator
        for (const ProjectionOperator* op : aops) {
            value_t op_value = mdp.get_action_cost(op);

            std::vector<StateRank> successors;

            Distribution<StateID> successor_dist;
            mdp.generate_action_transitions(s, op, successor_dist);

            for (const auto& [t, prob] : successor_dist) {
                op_value += prob * value_table[t.id];
                successors.push_back(StateRank(t));
            }

            if (is_approx_equal(value, op_value)) {
                greedy = op;
                greedy_successors = std::move(successors);
                goto found;
            }
        }

        abort();

    found:;
        const value_t cost_greedy = mdp.get_action_cost(greedy);

        // Generate successors
        for (const StateRank& succ : greedy_successors) {
            if (!mdp.is_goal(succ) && !closed.contains(succ)) {
                closed.insert(succ);
                open.push_back(succ);
            }
        }

        // Collect all equivalent greedy operators
        std::vector<PolicyDecision<const ProjectionOperator*>> decisions;

        for (const ProjectionOperator* op : aops) {
            const value_t cost_op = mdp.get_action_cost(op);

            if (are_equivalent(*op, *greedy) && cost_op == cost_greedy) {
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

} // namespace probfd::heuristics::pdbs