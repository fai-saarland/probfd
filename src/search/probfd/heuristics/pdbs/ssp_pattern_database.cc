#include "probfd/heuristics/pdbs/ssp_pattern_database.h"

#include "probfd/heuristics/pdbs/match_tree.h"

#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "probfd/engines/topological_value_iteration.h"

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

    virtual EvaluationResult evaluate(const StateRank& state) const
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
    bool operator_pruning,
    const StateRankEvaluator& heuristic)
    : ProbabilisticPatternDatabase(
          task_proxy,
          std::move(pattern),
          INFINITE_VALUE)
{
    ProjectionStateSpace state_space(
        task_proxy,
        ranking_function_,
        operator_pruning);
    compute_value_table(state_space, heuristic);
}

SSPPatternDatabase::SSPPatternDatabase(
    ProjectionStateSpace& state_space,
    StateRankingFunction ranking_function,
    const StateRankEvaluator& heuristic)
    : ProbabilisticPatternDatabase(std::move(ranking_function), INFINITE_VALUE)
{
    compute_value_table(state_space, heuristic);
}

SSPPatternDatabase::SSPPatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const ::pdbs::PatternDatabase& pdb,
    bool operator_pruning)
    : SSPPatternDatabase(
          task_proxy,
          pdb.get_pattern(),
          operator_pruning,
          PDBEvaluator(pdb))
{
}

SSPPatternDatabase::SSPPatternDatabase(
    ProjectionStateSpace& state_space,
    StateRankingFunction ranking_function,
    const ::pdbs::PatternDatabase& pdb)
    : SSPPatternDatabase(
          state_space,
          std::move(ranking_function),
          PDBEvaluator(pdb))
{
}

SSPPatternDatabase::SSPPatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const SSPPatternDatabase& pdb,
    int add_var,
    bool operator_pruning)
    : ProbabilisticPatternDatabase(
          task_proxy,
          utils::insert(pdb.get_pattern(), add_var),
          INFINITE_VALUE)
{
    ProjectionStateSpace state_space(
        task_proxy,
        ranking_function_,
        operator_pruning);
    compute_value_table(
        state_space,
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var));
}

SSPPatternDatabase::SSPPatternDatabase(
    ProjectionStateSpace& state_space,
    StateRankingFunction ranking_function,
    const SSPPatternDatabase& pdb,
    int add_var)
    : ProbabilisticPatternDatabase(std::move(ranking_function), INFINITE_VALUE)
{
    compute_value_table(
        state_space,
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var));
}

SSPPatternDatabase::SSPPatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const SSPPatternDatabase& left,
    const SSPPatternDatabase& right,
    bool operator_pruning)
    : ProbabilisticPatternDatabase(
          task_proxy,
          utils::merge_sorted(left.get_pattern(), right.get_pattern()),
          INFINITE_VALUE)
{
    ProjectionStateSpace state_space(
        task_proxy,
        ranking_function_,
        operator_pruning);
    compute_value_table(
        state_space,
        MergeEvaluator(ranking_function_, left, right));
}

SSPPatternDatabase::SSPPatternDatabase(
    ProjectionStateSpace& state_space,
    StateRankingFunction ranking_function,
    const SSPPatternDatabase& left,
    const SSPPatternDatabase& right)
    : ProbabilisticPatternDatabase(std::move(ranking_function), INFINITE_VALUE)
{
    compute_value_table(
        state_space,
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

std::unique_ptr<AbstractPolicy> SSPPatternDatabase::get_optimal_abstract_policy(
    ProjectionStateSpace& state_space,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool wildcard) const
{
    return ProbabilisticPatternDatabase::get_optimal_abstract_policy(
        state_space,
        rng,
        wildcard,
        true);
}

std::unique_ptr<AbstractPolicy>
SSPPatternDatabase::get_optimal_abstract_policy_no_traps(
    ProjectionStateSpace& state_space,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool wildcard) const
{
    return ProbabilisticPatternDatabase::get_optimal_abstract_policy_no_traps(
        state_space,
        rng,
        wildcard,
        true);
}

void SSPPatternDatabase::dump_graphviz(
    ProjectionStateSpace& state_space,
    const std::string& path,
    bool transition_labels) const
{
    auto s2str = [this](const StateRank& x) {
        std::ostringstream out;
        out.precision(3);
        out << x.id;

        const auto v = value_table[x.id];
        if (v == INFINITE_VALUE) {
            out << "\\nh = -&infin;";
        } else {
            out << "\\nh = " << v;
        }

        return out.str();
    };

    NormalCostAbstractCostFunction cost(
        state_space.goal_state_flags_,
        0_vt,
        INFINITE_VALUE);

    ProbabilisticPatternDatabase::dump_graphviz(
        state_space,
        path,
        s2str,
        cost,
        transition_labels);
}

void SSPPatternDatabase::compute_value_table(
    ProjectionStateSpace& state_space,
    const StateRankEvaluator& heuristic)
{
    using namespace preprocessing;
    using namespace engines::topological_vi;

    NormalCostAbstractCostFunction cost(
        state_space.goal_state_flags_,
        0_vt,
        INFINITE_VALUE);

    QualitativeReachabilityAnalysis<StateRank, const AbstractOperator*>
        analysis(
            &state_space.state_id_map,
            &state_space.action_id_map,
            &state_space.transition_gen,
            &cost,
            true);

    std::vector<StateID> proper_states;

    analysis.run_analysis(
        state_space.initial_state_,
        std::back_inserter(dead_ends_),
        std::back_inserter(proper_states));

    WrapperHeuristic h(proper_states, heuristic);

    TopologicalValueIteration<StateRank, const AbstractOperator*> vi(
        &state_space.state_id_map,
        &state_space.action_id_map,
        &state_space.transition_gen,
        &cost,
        &h,
        true);

    vi.solve(state_space.initial_state_.id, value_table);

#if !defined(NDEBUG)
    std::cout << "(II) Pattern [";
    for (unsigned i = 0; i < ranking_function_.get_pattern().size(); ++i) {
        std::cout << (i > 0 ? ", " : "") << ranking_function_.get_pattern()[i];
    }

    std::cout << "]: value=" << value_table[state_space.initial_state_.id]
              << std::endl;

#if defined(USE_LP)
    verify(state_space, proper_states);
#endif
#endif
}

#if !defined(NDEBUG) && defined(USE_LP)
void SSPPatternDatabase::verify(
    ProjectionStateSpace& state_space,
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

    for (StateRank s = StateRank(0);
         s.id != static_cast<int>(ranking_function_.num_states());
         ++s.id) {
        variables.emplace_back(0_vt, inf, 0_vt);
    }

    named_vector::NamedVector<lp::LPConstraint> constraints;

    std::deque<StateRank> queue({state_space.initial_state_});
    std::set<StateRank> seen({state_space.initial_state_});

    while (!queue.empty()) {
        StateRank s = queue.front();
        queue.pop_front();

        if (!utils::contains(proper_states, StateID(s.id))) {
            continue;
        }

        variables[s.id].objective_coefficient = 1_vt;

        if (state_space.goal_state_flags_[s.id]) {
            auto& g = constraints.emplace_back(0_vt, 0_vt);
            g.insert(s.id, 1_vt);
        }

        // Generate operators...
        std::vector<const AbstractOperator*> aops;
        state_space.match_tree_.get_applicable_operators(s, aops);

        // Push successors
        for (const AbstractOperator* op : aops) {
            value_t cost = op->cost;

            std::unordered_map<StateRank, value_t> successor_dist;

            for (const auto& [eff, prob] : op->outcomes) {
                successor_dist[s + eff] -= prob;
            }

            if (successor_dist.size() == 1 &&
                successor_dist.begin()->first == s) {
                continue;
            }

            successor_dist[s] += 1_vt;

            auto& constr = constraints.emplace_back(-inf, cost);

            for (const auto& [succ, prob] : successor_dist) {
                constr.insert(succ.id, prob);

                if (seen.insert(succ).second) {
                    queue.push_back(succ);
                }
            }
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

    for (StateRank s = StateRank(0);
         s.id != static_cast<int>(ranking_function_.num_states());
         ++s.id) {
        if (utils::contains(proper_states, StateID(s.id)) && seen.contains(s)) {
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
