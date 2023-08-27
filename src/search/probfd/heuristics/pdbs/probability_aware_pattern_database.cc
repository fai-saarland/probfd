#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/policies/vector_multi_policy.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/graph_visualization.h"

#include "downward/pdbs/pattern_database.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"

#include <algorithm>
#include <deque>
#include <numeric>
#include <set>
#include <sstream>
#include <unordered_set>

namespace probfd {
namespace heuristics {
namespace pdbs {

void ProbabilityAwarePatternDatabase::compute_value_table(
    ProjectionStateSpace& mdp,
    const StateRankEvaluator& h,
    StateRank init_state,
    double max_time)
{
    pdbs::compute_value_table(mdp, h, init_state, max_time, value_table);

#ifndef NDEBUG
    std::cout << "(II) Pattern " << ranking_function_.get_pattern() << ":\n"
              << "     value s0: " << value_table[init_state.id] << std::endl;
#endif
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    Pattern pattern,
    value_t dead_end_cost)
    : ranking_function_(task_proxy.get_variables(), std::move(pattern))
    , value_table(ranking_function_.num_states(), dead_end_cost)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    StateRankingFunction ranking_function,
    value_t dead_end_cost)
    : ranking_function_(std::move(ranking_function))
    , value_table(ranking_function_.num_states(), dead_end_cost)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    Pattern pattern,
    FDRSimpleCostFunction& task_cost_function,
    const State& initial_state,
    bool operator_pruning,
    const StateRankEvaluator& heuristic,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          std::move(pattern),
          task_cost_function.get_non_goal_termination_cost())
{
    utils::CountdownTimer timer(max_time);
    ProjectionStateSpace mdp(
        task_proxy,
        ranking_function_,
        task_cost_function,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        heuristic,
        ranking_function_.get_abstract_rank(initial_state),
        timer.get_remaining_time());
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& mdp,
    StateRankingFunction ranking_function,
    StateRank initial_state,
    const StateRankEvaluator& heuristic,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          std::move(ranking_function),
          mdp.get_non_goal_termination_cost())
{
    compute_value_table(mdp, heuristic, initial_state, max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const ::pdbs::PatternDatabase& pdb,
    FDRSimpleCostFunction& task_cost_function,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          pdb.get_pattern(),
          task_cost_function,
          initial_state,
          operator_pruning,
          task_cost_function.get_non_goal_termination_cost() == INFINITE_VALUE
              ? static_cast<const StateRankEvaluator&>(PDBEvaluator(pdb))
              : static_cast<const StateRankEvaluator&>(
                    DeadendPDBEvaluator(pdb)),
          max_time)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& mdp,
    StateRankingFunction ranking_function,
    StateRank initial_state,
    const ::pdbs::PatternDatabase& pdb,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          mdp,
          std::move(ranking_function),
          initial_state,
          mdp.get_non_goal_termination_cost() == INFINITE_VALUE
              ? static_cast<const StateRankEvaluator&>(PDBEvaluator(pdb))
              : static_cast<const StateRankEvaluator&>(
                    DeadendPDBEvaluator(pdb)),
          max_time)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const ProbabilityAwarePatternDatabase& pdb,
    int add_var,
    FDRSimpleCostFunction& task_cost_function,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          extended_pattern(pdb.get_pattern(), add_var),
          task_cost_function.get_non_goal_termination_cost())
{
    utils::CountdownTimer timer(max_time);

    ProjectionStateSpace mdp(
        task_proxy,
        ranking_function_,
        task_cost_function,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var),
        ranking_function_.get_abstract_rank(initial_state),
        timer.get_remaining_time());
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& mdp,
    StateRankingFunction ranking_function,
    StateRank initial_state,
    const ProbabilityAwarePatternDatabase& pdb,
    int add_var,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          std::move(ranking_function),
          mdp.get_non_goal_termination_cost())
{
    compute_value_table(
        mdp,
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var),
        initial_state,
        max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    FDRSimpleCostFunction& task_cost_function,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          utils::merge_sorted(left.get_pattern(), right.get_pattern()),
          task_cost_function.get_non_goal_termination_cost())
{
    utils::CountdownTimer timer(max_time);

    ProjectionStateSpace mdp(
        task_proxy,
        ranking_function_,
        task_cost_function,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        MergeEvaluator(
            ranking_function_,
            left,
            right,
            task_cost_function.get_non_goal_termination_cost()),
        ranking_function_.get_abstract_rank(initial_state),
        timer.get_remaining_time());
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& mdp,
    StateRankingFunction ranking_function,
    StateRank initial_state,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          std::move(ranking_function),
          mdp.get_non_goal_termination_cost())
{
    compute_value_table(
        mdp,
        MergeEvaluator(
            ranking_function_,
            left,
            right,
            mdp.get_non_goal_termination_cost()),
        initial_state,
        max_time);
}

const Pattern& ProbabilityAwarePatternDatabase::get_pattern() const
{
    return ranking_function_.get_pattern();
}

const StateRankingFunction&
ProbabilityAwarePatternDatabase::get_state_ranking_function() const
{
    return ranking_function_;
}

unsigned int ProbabilityAwarePatternDatabase::num_states() const
{
    return ranking_function_.num_states();
}

value_t ProbabilityAwarePatternDatabase::lookup_estimate(const State& s) const
{
    return lookup_estimate(get_abstract_state(s));
}

value_t ProbabilityAwarePatternDatabase::lookup_estimate(StateRank s) const
{
    return value_table[s.id];
}

StateRank
ProbabilityAwarePatternDatabase::get_abstract_state(const State& s) const
{
    return ranking_function_.get_abstract_rank(s);
}

std::unique_ptr<ProjectionMultiPolicy>
ProbabilityAwarePatternDatabase::compute_optimal_projection_policy(
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    utils::RandomNumberGenerator& rng,
    bool wildcard) const
{
    using PredecessorList =
        std::vector<std::pair<StateRank, const ProjectionOperator*>>;

    const value_t term_cost = mdp.get_non_goal_termination_cost();

    assert(lookup_estimate(initial_state) != term_cost);

    std::unique_ptr policy = std::make_unique<
        policies::VectorMultiPolicy<StateRank, const ProjectionOperator*>>(
        &mdp,
        ranking_function_.num_states());

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

        const value_t value = value_table[s.id];

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

            const value_t parent_cost = value_table[pstate.id];

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

            (*policy)[pstate.id] = std::move(decisions);
        }
    }

    return policy;
}

std::unique_ptr<ProjectionMultiPolicy>
ProbabilityAwarePatternDatabase::compute_greedy_projection_policy(
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    utils::RandomNumberGenerator& rng,
    bool wildcard) const
{
    const value_t term_cost = mdp.get_non_goal_termination_cost();

    std::unique_ptr policy = std::make_unique<
        policies::VectorMultiPolicy<StateRank, const ProjectionOperator*>>(
        &mdp,
        ranking_function_.num_states());

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

        const value_t value = value_table[s.id];

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
        (*policy)[s.id] = std::move(decisions);

        assert(!(*policy)[s.id].empty());
    }

    return policy;
}

void ProbabilityAwarePatternDatabase::compute_saturated_costs(
    ProjectionStateSpace& state_space,
    std::vector<value_t>& saturated_costs) const
{
    pdbs::compute_saturated_costs(state_space, value_table, saturated_costs);
}

void ProbabilityAwarePatternDatabase::dump_graphviz(
    const ProbabilisticTaskProxy& task_proxy,
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    std::ostream& out,
    bool transition_labels) const
{
    ProjectionOperatorToString op_names(task_proxy);

    auto sts = [this, &mdp](StateRank x) {
        std::ostringstream out;
        out.precision(3);

        const value_t value = value_table[x.id];
        std::string value_text =
            value != INFINITE_VALUE ? std::to_string(value) : "&infin";

        out << x.id << "\\n"
            << "h = " << value_text;

        if (lookup_estimate(x) == mdp.get_non_goal_termination_cost()) {
            out << "(dead)";
        }

        out << std::endl;

        return out.str();
    };

    auto ats = [=](const ProjectionOperator* const& op) {
        return transition_labels ? op_names(op) : "";
    };

    graphviz::dump_state_space_dot_graph<StateRank, const ProjectionOperator*>(
        out,
        initial_state,
        &mdp,
        nullptr,
        sts,
        ats,
        true);
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
