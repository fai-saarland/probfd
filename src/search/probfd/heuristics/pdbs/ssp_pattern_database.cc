#include "probfd/heuristics/pdbs/ssp_pattern_database.h"

#include "probfd/heuristics/pdbs/match_tree.h"

#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "probfd/engines/ta_topological_value_iteration.h"

#include "probfd/utils/graph_visualization.h"

#include "pdbs/pattern_database.h"

#include "utils/collections.h"

#include "lp/lp_solver.h"

#include <deque>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace probfd {
namespace heuristics {
namespace pdbs {

namespace {
class WrapperHeuristic : public StateRankEvaluator {
    const std::vector<StateID>& one_states;
    const StateRankEvaluator& parent;

public:
    WrapperHeuristic(
        const std::vector<StateID>& one_states,
        const StateRankEvaluator& parent)
        : one_states(one_states)
        , parent(parent)
    {
    }

    EvaluationResult evaluate(StateRank state) const override
    {
        if (utils::contains(one_states, StateID(state.id))) {
            return parent.evaluate(state);
        }

        return EvaluationResult{true, INFINITE_VALUE};
    }
};
} // namespace

SSPPatternDatabase::SSPPatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    Pattern pattern,
    TaskCostFunction& task_cost_function,
    const State& initial_state,
    bool operator_pruning,
    const StateRankEvaluator& heuristic)
    : ProbabilisticPatternDatabase(task_proxy, std::move(pattern))
{
    ProjectionStateSpace state_space(
        task_proxy,
        ranking_function_,
        operator_pruning);
    ProjectionCostFunction cost_function(
        task_proxy,
        ranking_function_,
        &task_cost_function);
    compute_value_table(
        state_space,
        cost_function,
        ranking_function_.rank(initial_state),
        heuristic);
}

SSPPatternDatabase::SSPPatternDatabase(
    ProjectionStateSpace& state_space,
    StateRankingFunction ranking_function,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const StateRankEvaluator& heuristic)
    : ProbabilisticPatternDatabase(std::move(ranking_function))
{
    compute_value_table(state_space, cost_function, initial_state, heuristic);
}

SSPPatternDatabase::SSPPatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const ::pdbs::PatternDatabase& pdb,
    TaskCostFunction& task_cost_function,
    const State& initial_state,
    bool operator_pruning)
    : SSPPatternDatabase(
          task_proxy,
          pdb.get_pattern(),
          task_cost_function,
          initial_state,
          operator_pruning,
          PDBEvaluator(pdb))
{
}

SSPPatternDatabase::SSPPatternDatabase(
    ProjectionStateSpace& state_space,
    StateRankingFunction ranking_function,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const ::pdbs::PatternDatabase& pdb)
    : SSPPatternDatabase(
          state_space,
          std::move(ranking_function),
          cost_function,
          initial_state,
          PDBEvaluator(pdb))
{
}

SSPPatternDatabase::SSPPatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const SSPPatternDatabase& pdb,
    int add_var,
    TaskCostFunction& task_cost_function,
    const State& initial_state,
    bool operator_pruning)
    : ProbabilisticPatternDatabase(
          task_proxy,
          utils::insert(pdb.get_pattern(), add_var))
{
    ProjectionStateSpace state_space(
        task_proxy,
        ranking_function_,
        operator_pruning);
    ProjectionCostFunction cost_function(
        task_proxy,
        ranking_function_,
        &task_cost_function);
    compute_value_table(
        state_space,
        cost_function,
        ranking_function_.rank(initial_state),
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var));
}

SSPPatternDatabase::SSPPatternDatabase(
    ProjectionStateSpace& state_space,
    StateRankingFunction ranking_function,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const SSPPatternDatabase& pdb,
    int add_var)
    : ProbabilisticPatternDatabase(std::move(ranking_function))
{
    compute_value_table(
        state_space,
        cost_function,
        initial_state,
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var));
}

SSPPatternDatabase::SSPPatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const SSPPatternDatabase& left,
    const SSPPatternDatabase& right,
    TaskCostFunction& task_cost_function,
    const State& initial_state,
    bool operator_pruning)
    : ProbabilisticPatternDatabase(
          task_proxy,
          utils::merge_sorted(left.get_pattern(), right.get_pattern()))
{
    ProjectionStateSpace state_space(
        task_proxy,
        ranking_function_,
        operator_pruning);
    ProjectionCostFunction cost_function(
        task_proxy,
        ranking_function_,
        &task_cost_function);
    compute_value_table(
        state_space,
        cost_function,
        ranking_function_.rank(initial_state),
        MergeEvaluator(ranking_function_, left, right));
}

SSPPatternDatabase::SSPPatternDatabase(
    ProjectionStateSpace& state_space,
    StateRankingFunction ranking_function,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const SSPPatternDatabase& left,
    const SSPPatternDatabase& right)
    : ProbabilisticPatternDatabase(std::move(ranking_function))
{
    compute_value_table(
        state_space,
        cost_function,
        initial_state,
        MergeEvaluator(ranking_function_, left, right));
}

EvaluationResult SSPPatternDatabase::evaluate(const State& s) const
{
    return evaluate(get_abstract_state(s));
}

EvaluationResult SSPPatternDatabase::evaluate(StateRank s) const
{
    const auto v = this->lookup(s);
    return {v == INFINITE_VALUE, v};
}

void SSPPatternDatabase::compute_value_table(
    ProjectionStateSpace& state_space,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const StateRankEvaluator& heuristic)
{
    using namespace preprocessing;
    using namespace engines::ta_topological_vi;

    QualitativeReachabilityAnalysis<StateRank, const AbstractOperator*>
        analysis(&state_space, &cost_function, true);

    std::vector<StateID> proper_states;

    analysis.run_analysis(
        initial_state,
        std::back_inserter(dead_ends_),
        std::back_inserter(proper_states));

    WrapperHeuristic h(proper_states, heuristic);

    TATopologicalValueIteration<StateRank, const AbstractOperator*> vi(
        &state_space,
        &cost_function,
        &h);

    vi.solve(initial_state.id, value_table);

#if !defined(NDEBUG)
    std::cout << "(II) Pattern [";
    for (unsigned i = 0; i < ranking_function_.get_pattern().size(); ++i) {
        std::cout << (i > 0 ? ", " : "") << ranking_function_.get_pattern()[i];
    }

    std::cout << "]: value=" << value_table[initial_state.id] << std::endl;

#if defined(USE_LP)
    verify(state_space, cost_function, initial_state, proper_states);
#endif
#endif
}

#if !defined(NDEBUG) && defined(USE_LP)
void SSPPatternDatabase::verify(
    ProjectionStateSpace& state_space,
    ProjectionCostFunction& cost_function,
    StateRank initial_state,
    const std::vector<StateID>& proper_states)
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
        variables.emplace_back(0_vt, INFINITE_VALUE, 1_vt);
    }

    named_vector::NamedVector<lp::LPConstraint> constraints;

    std::deque<StateID> queue({state_space.get_state_id(initial_state)});
    std::set<StateID> seen({state_space.get_state_id(initial_state)});

    while (!queue.empty()) {
        StateID s = queue.front();
        queue.pop_front();

        if (!utils::contains(proper_states, s)) {
            continue;
        }

        if (cost_function.is_goal(state_space.get_state(s))) {
            auto& g = constraints.emplace_back(0_vt, 0_vt);
            g.insert(s.id, 1_vt);
        }

        // Generate operators...
        std::vector<const AbstractOperator*> aops;
        state_space.generate_applicable_actions(s.id, aops);

        // Push successors
        for (const AbstractOperator* op : aops) {
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

    assert(solver.has_optimal_solution());

    std::vector<double> solution = solver.extract_solution();

    for (StateID s = 0; s != num_states; ++s.id) {
        if (utils::contains(proper_states, s) && seen.contains(s)) {
            assert(is_approx_equal(solution[s.id], value_table[s.id], 0.001));
        } else {
            assert(value_table[s.id] == INFINITE_VALUE);
        }
    }
}
#endif

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
