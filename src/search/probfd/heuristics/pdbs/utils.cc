#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/distribution.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"

#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/rng.h"

#if !defined(NDEBUG) && defined(USE_LP)
#include "downward/lp/lp_solver.h"
#endif

#include <cassert>
#include <deque>
#include <limits>
#include <ranges>

namespace probfd {
namespace heuristics {
namespace pdbs {

Pattern extended_pattern(const Pattern& pattern, int add_var)
{
    assert(!utils::contains(pattern, add_var));

    Pattern added;
    added.reserve(pattern.size() + 1);
    auto it = std::upper_bound(pattern.begin(), pattern.end(), add_var);
    added.insert(added.end(), pattern.begin(), it);
    added.emplace_back(add_var);
    added.insert(added.end(), it, pattern.end());

    return added;
}

std::vector<int> get_goals_in_random_order(
    const ProbabilisticTaskProxy& task_proxy,
    utils::RandomNumberGenerator& rng)
{
    std::vector<int> goals;
    
    for (const auto fact : task_proxy.get_goals()) {
        goals.push_back(fact.get_variable().get_id());
    }

    rng.shuffle(goals);

    return goals;
}

std::ostream& operator<<(std::ostream& out, const Pattern& pattern)
{
    if (pattern.empty()) return out << "[]";

    out << "[" << pattern.front();

    for (int var : pattern | std::views::drop(1)) {
        out << ", " << var;
    }

    return out << "]";
}

#if !defined(NDEBUG) && defined(USE_LP)
void verify(
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    std::vector<value_t>& value_table,
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

    const size_t num_states = value_table.size();

    for (size_t i = 0; i != num_states; ++i) {
        variables.emplace_back(0_vt, std::min(term_cost, inf), 0_vt);
    }

    named_vector::NamedVector<lp::LPConstraint> constraints;

    std::deque<StateRank> queue({initial_state});
    std::set<StateRank> seen({initial_state});

    while (!queue.empty()) {
        StateRank s = queue.front();
        queue.pop_front();

        if (utils::contains(pruned_states, s.id)) {
            continue;
        }

        variables[s.id].objective_coefficient = 1_vt;

        if (mdp.is_goal(s)) {
            auto& g = constraints.emplace_back(0_vt, 0_vt);
            g.insert(s.id, 1_vt);
        }

        // Generate operators...
        std::vector<const ProjectionOperator*> aops;
        mdp.generate_applicable_actions(s, aops);

        // Push successors
        for (const ProjectionOperator* op : aops) {
            const value_t cost = mdp.get_action_cost(op);

            Distribution<StateID> successor_dist;
            mdp.generate_action_transitions(s, op, successor_dist);

            if (successor_dist.is_dirac(s.id)) {
                continue;
            }

            auto& constr = constraints.emplace_back(-inf, cost);

            value_t non_loop_prob = 0_vt;
            for (const auto& [succ, prob] : successor_dist) {
                if (succ != s.id) {
                    non_loop_prob += prob;
                    constr.insert(succ.id, -prob);
                }

                if (seen.insert(mdp.get_state(succ)).second) {
                    queue.push_back(mdp.get_state(succ));
                }
            }

            constr.insert(s.id, non_loop_prob);
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

    for (StateRank s{0}; s.id != num_states; ++s.id) {
        if (utils::contains(pruned_states, s.id) || !seen.contains(s)) {
            assert(value_table[s.id] == term_cost);
        } else {
            assert(is_approx_equal(solution[s.id], value_table[s.id], 0.001));
        }
    }
}
#endif

void compute_value_table(
    ProjectionStateSpace& mdp,
    const StateRankEvaluator& heuristic,
    StateRank initial_state,
    double max_time,
    std::vector<value_t>& value_table)
{
    using namespace preprocessing;
    using namespace algorithms::ta_topological_vi;

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
            if (utils::contains(pruned_states, StateID(state.id))) {
                return pruned_value;
            }

            return parent.evaluate(state);
        }
    };

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
    vi.solve(mdp, h, initial_state.id, value_table, timer.get_remaining_time());

#if !defined(NDEBUG) && defined(USE_LP)
    verify(mdp, initial_state, value_table, pruned_states);
#endif
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
