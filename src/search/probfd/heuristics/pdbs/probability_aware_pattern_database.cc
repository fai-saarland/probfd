#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"
#include "probfd/policies/vector_multi_policy.h"

#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/value_type.h"

#include "downward/pdbs/pattern_database.h"

#include "downward/lp/lp_solver.h"

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

namespace {
class WrapperHeuristic : public StateRankEvaluator {
    const std::vector<StateID>& pruned_states;
    const StateRankEvaluator& parent;
    const value_t pruned_value;

public:
    WrapperHeuristic(
        const std::vector<StateID>& pruned_states,
        const StateRankEvaluator& parent,
        value_t pruned_value)
        : pruned_states(pruned_states)
        , parent(parent)
        , pruned_value(pruned_value)
    {
    }

    value_t evaluate(StateRank state) const override
    {
        if (utils::contains(pruned_states, state)) {
            return pruned_value;
        }

        return parent.evaluate(state);
    }
};
} // namespace

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProbabilisticTaskProxy task_proxy,
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
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    Pattern pattern,
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
        task_cost_function,
        ranking_function_,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        ranking_function_.get_abstract_rank(initial_state),
        heuristic,
        value_table,
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
    compute_value_table(mdp, initial_state, heuristic, value_table, max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const ::pdbs::PatternDatabase& pdb,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          task_cost_function,
          pdb.get_pattern(),
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
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const ProbabilityAwarePatternDatabase& pdb,
    int add_var,
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
        task_cost_function,
        ranking_function_,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        ranking_function_.get_abstract_rank(initial_state),
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var),
        value_table,
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
        initial_state,
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var),
        value_table,
        max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
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
        task_cost_function,
        ranking_function_,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        ranking_function_.get_abstract_rank(initial_state),
        MergeEvaluator(
            ranking_function_,
            left,
            right,
            task_cost_function.get_non_goal_termination_cost()),
        value_table,
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
        initial_state,
        MergeEvaluator(
            ranking_function_,
            left,
            right,
            mdp.get_non_goal_termination_cost()),
        value_table,
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

const std::vector<value_t>&
ProbabilityAwarePatternDatabase::get_value_table() const
{
    return value_table;
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
    return value_table[s];
}

StateRank
ProbabilityAwarePatternDatabase::get_abstract_state(const State& s) const
{
    return ranking_function_.get_abstract_rank(s);
}

std::unique_ptr<ProjectionMultiPolicy> compute_optimal_projection_policy(
    ProjectionStateSpace& mdp,
    const std::vector<value_t>& value_table,
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
    const std::vector<value_t>& value_table,
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

void compute_saturated_costs(
    ProjectionStateSpace& state_space,
    const std::vector<value_t>& value_table,
    std::vector<value_t>& saturated_costs)
{
    std::fill(saturated_costs.begin(), saturated_costs.end(), -INFINITE_VALUE);

    const int num_states = value_table.size();
    for (StateRank s = 0; s < num_states; ++s) {
        const value_t value = value_table[s];

        if (value == INFINITE_VALUE) {
            // Skip dead ends and unreachable states
            continue;
        }

        // Generate operators...
        std::vector<const ProjectionOperator*> aops;
        state_space.generate_applicable_actions(s, aops);

        for (const ProjectionOperator* op : aops) {
            int oid = op->operator_id.get_index();

            Distribution<StateID> successor_dist;
            state_space.generate_action_transitions(s, op, successor_dist);

            value_t h_succ = 0;

            for (const auto& [t, prob] : successor_dist) {
                const auto succ_val = value_table[t];

                if (succ_val == INFINITE_VALUE) {
                    // No need to consider dead transitions.
                    goto next_operator;
                }

                h_succ += prob * succ_val;
            }

            saturated_costs[oid] =
                std::max(saturated_costs[oid], value - h_succ);

        next_operator:;
        }
    }
}

void ProbabilityAwarePatternDatabase::dump_graphviz(
    ProbabilisticTaskProxy task_proxy,
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    std::ostream& out,
    bool transition_labels) const
{
    ProjectionOperatorToString op_names(task_proxy);

    auto sts = [this, &mdp](StateRank x) {
        std::ostringstream out;
        out.precision(3);

        const value_t value = value_table[x];
        std::string value_text =
            value != INFINITE_VALUE ? std::to_string(value) : "&infin";

        out << x << "\\n"
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

void compute_value_table(
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    const StateRankEvaluator& heuristic,
    std::vector<value_t>& value_table,
    double max_time)
{
    using namespace preprocessing;
    using namespace algorithms::ta_topological_vi;

    utils::CountdownTimer timer(max_time);

    QualitativeReachabilityAnalysis<StateRank, const ProjectionOperator*>
        analysis(true);

    std::vector<StateID> pruned_states;

    if (mdp.get_non_goal_termination_cost() == INFINITE_VALUE) {
        analysis.run_analysis(
            mdp,
            nullptr,
            initial_state,
            iterators::discarding_output_iterator{},
            std::back_inserter(pruned_states),
            iterators::discarding_output_iterator{},
            timer.get_remaining_time());
    } else {
        analysis.run_analysis(
            mdp,
            nullptr,
            initial_state,
            std::back_inserter(pruned_states),
            iterators::discarding_output_iterator{},
            iterators::discarding_output_iterator{},
            timer.get_remaining_time());
    }

    WrapperHeuristic h(
        pruned_states,
        heuristic,
        mdp.get_non_goal_termination_cost());

    TATopologicalValueIteration<StateRank, const ProjectionOperator*> vi;
    vi.solve(mdp, h, initial_state, value_table, timer.get_remaining_time());

#if !defined(NDEBUG) && defined(USE_LP)
    verify(mdp, initial_state, value_table, pruned_states);
#endif
}

#if !defined(NDEBUG) && defined(USE_LP)
void ProbabilityAwarePatternDatabase::verify(
    ProjectionStateSpace& mdp,
    const std::vector<value_t>& value_table,
    StateRank initial_state,
    const std::vector<StateID>& pruned_states)
{
    lp::LPSolverType type;

#ifdef COIN_HAS_CLP
    type = lp::LPSolverType::CLP;
#elif defined(COIN_HAS_CPX)
    type = lp::LPSolverType::CPLEX;
#elif defined(COIN_HAS_GRB)
    type = lp::LPSolverType::GUROBI;
#elif defined(COIN_HAS_SPX)
    type = lp::LPSolverType::SOPLEX;
#else
    std::cerr << "Warning: Could not verify PDB value table since no LP solver"
                 "is available !"
              << std::endl;
    return;
#endif

    lp::LPSolver solver(type);
    const double inf = solver.get_infinity();
    const value_t term_cost = mdp.get_non_goal_termination_cost();

    named_vector::NamedVector<lp::LPVariable> variables;

    const size_t num_states = ranking_function_.num_states();

    for (size_t i = 0; i != num_states; ++i) {
        variables.emplace_back(0_vt, std::min(term_cost, inf), 0_vt);
    }

    named_vector::NamedVector<lp::LPConstraint> constraints;

    std::deque<StateRank> queue({initial_state});
    std::set<StateRank> seen({initial_state});

    while (!queue.empty()) {
        StateRank s = queue.front();
        queue.pop_front();

        if (utils::contains(pruned_states, s)) {
            continue;
        }

        variables[s].objective_coefficient = 1_vt;

        if (mdp.is_goal(s)) {
            auto& g = constraints.emplace_back(0_vt, 0_vt);
            g.insert(s, 1_vt);
        }

        // Generate operators...
        std::vector<const ProjectionOperator*> aops;
        mdp.generate_applicable_actions(s, aops);

        // Push successors
        for (const ProjectionOperator* op : aops) {
            const value_t cost = mdp.get_action_cost(op);

            Distribution<StateID> successor_dist;
            mdp.generate_action_transitions(s, op, successor_dist);

            if (successor_dist.is_dirac(s)) {
                continue;
            }

            auto& constr = constraints.emplace_back(-inf, cost);

            value_t non_loop_prob = 0_vt;
            for (const auto& [succ, prob] : successor_dist) {
                if (succ != s) {
                    non_loop_prob += prob;
                    constr.insert(succ.id, -prob);
                }

                if (seen.insert(mdp.get_state(succ)).second) {
                    queue.push_back(mdp.get_state(succ));
                }
            }

            constr.insert(s, non_loop_prob);
        }
    }

    solver.load_problem(lp::LinearProgram(
        lp::LPObjectiveSense::MAXIMIZE,
        std::move(variables),
        std::move(constraints),
        inf));

    solver.solve();

    if (!solver.has_optimal_solution()) {
        if (solver.is_infeasible()) {
            std::cerr << "Critical error: LP was infeasible!" << std::endl;
        } else {
            assert(solver.is_unbounded());
            std::cerr << "Critical error: LP was unbounded!" << std::endl;
        }

        solver.print_failure_analysis();
        abort();
    }

    std::vector<double> solution = solver.extract_solution();

    for (StateRank s = 0; s != num_states; ++s) {
        if (utils::contains(pruned_states, s) || !seen.contains(s)) {
            assert(value_table[s] == term_cost);
        } else {
            assert(is_approx_equal(solution[s], value_table[s], 0.001));
        }
    }
}
#endif

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
