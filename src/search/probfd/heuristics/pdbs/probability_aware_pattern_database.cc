#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/engines/ta_topological_value_iteration.h"

#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/value_type.h"

#include "pdbs/pattern_database.h"

#include "lp/lp_solver.h"

#include "utils/collections.h"
#include "utils/countdown_timer.h"

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

    EvaluationResult evaluate(StateRank state) const override
    {
        if (utils::contains(pruned_states, StateID(state.id))) {
            return EvaluationResult{true, pruned_value};
        }

        return parent.evaluate(state);
    }
};
} // namespace

void ProbabilityAwarePatternDatabase::compute_value_table(
    ProjectionStateSpace& state_space,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const StateRankEvaluator& heuristic,
    double max_time)
{
    using namespace preprocessing;
    using namespace engines::ta_topological_vi;

    utils::CountdownTimer timer(max_time);

    QualitativeReachabilityAnalysis<StateRank, const ProjectionOperator*>
        analysis(true);

    std::vector<StateID> pruned_states;

    if (dead_end_cost == INFINITE_VALUE) {
        analysis.run_analysis(
            {state_space, cost_function},
            initial_state,
            iterators::discarding_output_iterator{},
            std::back_inserter(pruned_states),
            iterators::discarding_output_iterator{},
            timer.get_remaining_time());
    } else {
        analysis.run_analysis(
            {state_space, cost_function},
            initial_state,
            std::back_inserter(pruned_states),
            iterators::discarding_output_iterator{},
            iterators::discarding_output_iterator{},
            timer.get_remaining_time());
    }

    WrapperHeuristic h(pruned_states, heuristic, dead_end_cost);

    TATopologicalValueIteration<StateRank, const ProjectionOperator*> vi(
        &state_space,
        &cost_function,
        &h);

    vi.solve(initial_state.id, value_table, timer.get_remaining_time());

#if !defined(NDEBUG)
    std::cout << "(II) Pattern [";
    for (unsigned i = 0; i < ranking_function_.get_pattern().size(); ++i) {
        std::cout << (i > 0 ? ", " : "") << ranking_function_.get_pattern()[i];
    }

    std::cout << "]: value=" << value_table[initial_state.id] << std::endl;

#if defined(USE_LP)
    verify(state_space, cost_function, initial_state, pruned_states);
#endif
#endif
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    Pattern pattern,
    value_t dead_end_cost)
    : ranking_function_(task_proxy.get_variables(), std::move(pattern))
    , value_table(ranking_function_.num_states(), dead_end_cost)
    , dead_end_cost(dead_end_cost)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    StateRankingFunction ranking_function,
    value_t dead_end_cost)
    : ranking_function_(std::move(ranking_function))
    , value_table(ranking_function_.num_states(), dead_end_cost)
    , dead_end_cost(dead_end_cost)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    Pattern pattern,
    TaskCostFunction& task_cost_function,
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
    ProjectionStateSpace state_space(
        task_proxy,
        ranking_function_,
        task_cost_function,
        operator_pruning,
        timer.get_remaining_time());
    InducedProjectionCostFunction cost_function(
        task_proxy,
        ranking_function_,
        &task_cost_function);
    compute_value_table(
        state_space,
        cost_function,
        ranking_function_.get_abstract_rank(initial_state),
        heuristic,
        timer.get_remaining_time());
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& state_space,
    StateRankingFunction ranking_function,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const StateRankEvaluator& heuristic,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          std::move(ranking_function),
          cost_function.get_non_goal_termination_cost())
{
    compute_value_table(
        state_space,
        cost_function,
        initial_state,
        heuristic,
        max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const ::pdbs::PatternDatabase& pdb,
    TaskCostFunction& task_cost_function,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          pdb.get_pattern(),
          task_cost_function,
          initial_state,
          operator_pruning,
          PDBEvaluator(pdb),
          max_time)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& state_space,
    StateRankingFunction ranking_function,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const ::pdbs::PatternDatabase& pdb,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          state_space,
          std::move(ranking_function),
          cost_function,
          initial_state,
          PDBEvaluator(pdb),
          max_time)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const ProbabilityAwarePatternDatabase& pdb,
    int add_var,
    TaskCostFunction& task_cost_function,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          extended_pattern(pdb.get_pattern(), add_var),
          task_cost_function.get_non_goal_termination_cost())
{
    utils::CountdownTimer timer(max_time);

    ProjectionStateSpace state_space(
        task_proxy,
        ranking_function_,
        task_cost_function,
        operator_pruning,
        timer.get_remaining_time());
    InducedProjectionCostFunction cost_function(
        task_proxy,
        ranking_function_,
        &task_cost_function);
    compute_value_table(
        state_space,
        cost_function,
        ranking_function_.get_abstract_rank(initial_state),
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var),
        timer.get_remaining_time());
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& state_space,
    StateRankingFunction ranking_function,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const ProbabilityAwarePatternDatabase& pdb,
    int add_var,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          std::move(ranking_function),
          cost_function.get_non_goal_termination_cost())
{
    compute_value_table(
        state_space,
        cost_function,
        initial_state,
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var),
        max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    TaskCostFunction& task_cost_function,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          utils::merge_sorted(left.get_pattern(), right.get_pattern()),
          task_cost_function.get_non_goal_termination_cost())
{
    utils::CountdownTimer timer(max_time);

    ProjectionStateSpace state_space(
        task_proxy,
        ranking_function_,
        task_cost_function,
        operator_pruning,
        timer.get_remaining_time());
    InducedProjectionCostFunction cost_function(
        task_proxy,
        ranking_function_,
        &task_cost_function);
    compute_value_table(
        state_space,
        cost_function,
        ranking_function_.get_abstract_rank(initial_state),
        MergeEvaluator(ranking_function_, left, right),
        timer.get_remaining_time());
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& state_space,
    StateRankingFunction ranking_function,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          std::move(ranking_function),
          cost_function.get_non_goal_termination_cost())
{
    compute_value_table(
        state_space,
        cost_function,
        initial_state,
        MergeEvaluator(ranking_function_, left, right),
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

bool ProbabilityAwarePatternDatabase::is_dead_end(const State& s) const
{
    return is_dead_end(get_abstract_state(s));
}

bool ProbabilityAwarePatternDatabase::is_dead_end(StateRank s) const
{
    return lookup_estimate(s) == dead_end_cost;
}

value_t ProbabilityAwarePatternDatabase::lookup_estimate(const State& s) const
{
    return lookup_estimate(get_abstract_state(s));
}

value_t ProbabilityAwarePatternDatabase::lookup_estimate(StateRank s) const
{
    return value_table[s.id];
}

EvaluationResult ProbabilityAwarePatternDatabase::evaluate(const State& s) const
{
    return evaluate(get_abstract_state(s));
}

EvaluationResult ProbabilityAwarePatternDatabase::evaluate(StateRank s) const
{
    const value_t value = this->lookup_estimate(s);
    return {value == dead_end_cost, value};
}

StateRank
ProbabilityAwarePatternDatabase::get_abstract_state(const State& s) const
{
    return ranking_function_.get_abstract_rank(s);
}

std::unique_ptr<ProjectionPolicy>
ProbabilityAwarePatternDatabase::compute_optimal_projection_policy(
    ProjectionStateSpace& state_space,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool wildcard) const
{
    using PredecessorList =
        std::vector<std::pair<StateRank, const ProjectionOperator*>>;

    assert(!is_dead_end(initial_state));

    ProjectionPolicy* policy =
        new ProjectionPolicy(ranking_function_.num_states());

    // return empty policy indicating unsolvable
    if (cost_function.is_goal(initial_state)) {
        return std::unique_ptr<ProjectionPolicy>(policy);
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

        // Skip dead-ends, the operator is irrelevant
        if (is_dead_end(s)) {
            continue;
        }

        const value_t value = value_table[s.id];

        // Generate operators...
        std::vector<const ProjectionOperator*> aops;
        state_space.generate_applicable_actions(s.id, aops);

        // Select the greedy operators and add their successors
        for (const ProjectionOperator* op : aops) {
            value_t op_value = cost_function.get_action_cost(op);

            std::vector<StateRank> successors;

            Distribution<StateID> successor_dist;
            state_space.generate_action_transitions(s.id, op, successor_dist);

            for (const auto& [t, prob] : successor_dist) {
                op_value += prob * value_table[t.id];
                successors.push_back(StateRank(t));
            }

            if (is_approx_equal(value, op_value)) {
                for (const StateRank succ : successors) {
                    if (cost_function.is_goal(succ)) {
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
        auto it = rng->choose(open);
        StateRank s = *it;

        std::swap(*it, open.back());
        open.pop_back();

        // Consider predecessors in random order
        rng->shuffle(predecessors[s]);

        for (const auto& [pstate, sel_op] : predecessors[s]) {
            if (closed.insert(pstate).second) {
                open.push_back(pstate);

                // Collect all equivalent greedy operators
                std::vector<const ProjectionOperator*> aops;
                state_space.generate_applicable_actions(pstate.id, aops);

                std::vector<const ProjectionOperator*> equivalent_operators;

                for (const ProjectionOperator* op : aops) {
                    if (are_equivalent(*op, *sel_op)) {
                        equivalent_operators.push_back(op);
                    }
                }

                // If wildcard consider all, else randomly pick one
                if (wildcard) {
                    (*policy)[pstate].insert(
                        (*policy)[pstate].end(),
                        equivalent_operators.begin(),
                        equivalent_operators.end());
                } else {
                    (*policy)[pstate].push_back(
                        *rng->choose(equivalent_operators));
                }
            }
        }
    }

    return std::unique_ptr<ProjectionPolicy>(policy);
}

std::unique_ptr<ProjectionPolicy>
ProbabilityAwarePatternDatabase::compute_greedy_projection_policy(
    ProjectionStateSpace& state_space,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool wildcard) const
{
    ProjectionPolicy* policy =
        new ProjectionPolicy(ranking_function_.num_states());

    if (cost_function.is_goal(initial_state)) {
        return std::unique_ptr<ProjectionPolicy>(policy);
    }

    std::deque<StateRank> open;
    std::unordered_set<StateRank> closed;
    open.push_back(initial_state);
    closed.insert(initial_state);

    // Build the greedy policy graph
    while (!open.empty()) {
        StateRank s = open.front();
        open.pop_front();

        // Skip dead-ends, the operator is irrelevant
        if (is_dead_end(s)) {
            continue;
        }

        const value_t value = value_table[s.id];

        // Generate operators...
        std::vector<const ProjectionOperator*> aops;
        state_space.generate_applicable_actions(s.id, aops);

        if (aops.empty()) {
            continue;
        }

        // Look at the (greedy) operators in random order.
        rng->shuffle(aops);

        const ProjectionOperator* greedy_operator = nullptr;
        std::vector<StateRank> greedy_successors;

        // Select first greedy operator
        for (const ProjectionOperator* op : aops) {
            value_t op_value = cost_function.get_action_cost(op);

            std::vector<StateRank> successors;

            Distribution<StateID> successor_dist;
            state_space.generate_action_transitions(s.id, op, successor_dist);

            for (const auto& [t, prob] : successor_dist) {
                op_value += prob * value_table[t.id];
                successors.push_back(StateRank(t));
            }

            if (is_approx_equal(value, op_value)) {
                greedy_operator = op;
                greedy_successors = std::move(successors);
                break;
            }
        }

        assert(greedy_operator != nullptr);

        // Generate successors
        for (const StateRank& succ : greedy_successors) {
            if (!cost_function.is_goal(succ) && !closed.contains(succ)) {
                closed.insert(succ);
                open.push_back(succ);
            }
        }

        // Collect all equivalent greedy operators
        std::vector<const ProjectionOperator*> equivalent_operators;

        for (const ProjectionOperator* op : aops) {
            if (are_equivalent(*op, *greedy_operator)) {
                equivalent_operators.push_back(op);
            }
        }

        // If wildcard consider all, else randomly pick one
        if (wildcard) {
            (*policy)[s].insert(
                (*policy)[s].end(),
                equivalent_operators.begin(),
                equivalent_operators.end());
        } else {
            (*policy)[s].push_back(*rng->choose(equivalent_operators));
        }

        assert(!(*policy)[s].empty());
    }

    return std::unique_ptr<ProjectionPolicy>(policy);
}

void ProbabilityAwarePatternDatabase::compute_saturated_costs(
    ProjectionStateSpace& state_space,
    std::vector<value_t>& saturated_costs) const
{
    std::fill(saturated_costs.begin(), saturated_costs.end(), -INFINITE_VALUE);

    for (StateRank s{0}; s.id < ranking_function_.num_states(); ++s.id) {
        const value_t value = value_table[s.id];

        if (value == INFINITE_VALUE) {
            // Skip dead ends and unreachable states
            continue;
        }

        // Generate operators...
        std::vector<const ProjectionOperator*> aops;
        state_space.generate_applicable_actions(s.id, aops);

        for (const ProjectionOperator* op : aops) {
            int oid = op->operator_id.get_index();

            Distribution<StateID> successor_dist;
            state_space.generate_action_transitions(s.id, op, successor_dist);

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
    const ProbabilisticTaskProxy& task_proxy,
    ProjectionStateSpace& state_space,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    std::ostream& out,
    bool transition_labels) const
{
    using namespace engine_interfaces;

    ProjectionOperatorToString op_names(task_proxy);

    auto sts = [this](StateRank x) {
        std::ostringstream out;
        out.precision(3);

        const value_t value = value_table[x.id];
        std::string value_text =
            value != INFINITE_VALUE ? std::to_string(value) : "&infin";

        out << x.id << "\\n"
            << "h = " << value_text;

        if (is_dead_end(x)) {
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
        &state_space,
        &cost_function,
        nullptr,
        sts,
        ats,
        true);
}

#if !defined(NDEBUG) && defined(USE_LP)
void ProbabilityAwarePatternDatabase::verify(
    ProjectionStateSpace& state_space,
    ProjectionCostFunction& cost_function,
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

    named_vector::NamedVector<lp::LPVariable> variables;

    const size_t num_states = ranking_function_.num_states();

    for (size_t i = 0; i != num_states; ++i) {
        variables.emplace_back(0_vt, std::min(dead_end_cost, inf), 0_vt);
    }

    named_vector::NamedVector<lp::LPConstraint> constraints;

    std::deque<StateID> queue({state_space.get_state_id(initial_state)});
    std::set<StateID> seen({state_space.get_state_id(initial_state)});

    while (!queue.empty()) {
        StateID s = queue.front();
        queue.pop_front();

        if (utils::contains(pruned_states, s)) {
            continue;
        }

        variables[s.id].objective_coefficient = 1_vt;

        if (cost_function.is_goal(state_space.get_state(s))) {
            auto& g = constraints.emplace_back(0_vt, 0_vt);
            g.insert(s.id, 1_vt);
        }

        // Generate operators...
        std::vector<const ProjectionOperator*> aops;
        state_space.generate_applicable_actions(s.id, aops);

        // Push successors
        for (const ProjectionOperator* op : aops) {
            const value_t cost = cost_function.get_action_cost(op);

            Distribution<StateID> successor_dist;
            state_space.generate_action_transitions(s.id, op, successor_dist);

            if (successor_dist.is_dirac(s.id)) {
                continue;
            }

            auto& constr = constraints.emplace_back(-inf, cost);

            value_t non_loop_prob = 0_vt;
            for (const auto& [succ, prob] : successor_dist) {
                if (succ != s) {
                    non_loop_prob += prob;
                    constr.insert(succ, -prob);
                }

                if (seen.insert(succ).second) {
                    queue.push_back(succ);
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

    for (StateID s = 0; s != num_states; ++s.id) {
        if (utils::contains(pruned_states, s) || !seen.contains(s)) {
            assert(value_table[s.id] == dead_end_cost);
        } else {
            assert(is_approx_equal(solution[s.id], value_table[s.id], 0.001));
        }
    }
}
#endif

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
