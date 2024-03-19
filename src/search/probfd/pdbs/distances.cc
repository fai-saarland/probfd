#include "probfd/pdbs/distances.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/evaluator.h"

#include "downward/utils/countdown_timer.h"

#include <ranges>

#if !defined(NDEBUG) && (defined(HAS_CPLEX) || defined(HAS_SOPLEX))
#include "downward/lp/lp_solver.h"

#include <deque>
#include <set>
#endif

namespace probfd::pdbs {

#if !defined(NDEBUG) && (defined(HAS_CPLEX) || defined(HAS_SOPLEX))
static void verify(
    ProjectionStateSpace& mdp,
    std::span<const value_t> value_table,
    StateRank initial_state)
{
    lp::LPSolverType type;

#ifdef HAS_CPLEX
    type = lp::LPSolverType::CPLEX;
#else
    type = lp::LPSolverType::SOPLEX;
#endif

    lp::LPSolver solver(type);
    const double inf = solver.get_infinity();
    const value_t term_cost = mdp.get_non_goal_termination_cost();

    named_vector::NamedVector<lp::LPVariable> variables;

    const int num_states = static_cast<int>(value_table.size());

    for (int i = 0; i != num_states; ++i) {
        variables.emplace_back(0_vt, std::min(term_cost, inf), 0_vt);
    }

    named_vector::NamedVector<lp::LPConstraint> constraints;

    std::deque<StateRank> queue({initial_state});
    std::set<StateRank> seen({initial_state});

    while (!queue.empty()) {
        StateRank s = queue.front();
        queue.pop_front();

        if (value_table[s] == INFINITE_VALUE) {
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
                if (succ != static_cast<size_t>(s)) {
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
        if (!seen.contains(s)) {
            assert(value_table[s] == -INFINITE_VALUE);
        } else if (value_table[s] != INFINITE_VALUE) {
            assert(is_approx_equal(solution[s], value_table[s], 0.001));
        }
    }
}
#endif

void compute_value_table(
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    const Evaluator<StateRank>& heuristic,
    std::span<value_t> value_table,
    double max_time)
{
    using namespace algorithms::ta_topological_vi;

    utils::CountdownTimer timer(max_time);

    TATopologicalValueIteration<StateRank, const ProjectionOperator*> vi(
        value_table.size());
    vi.solve(
        mdp,
        heuristic,
        initial_state,
        value_table,
        timer.get_remaining_time());

    std::cout << " Initial state value: " << value_table[initial_state]
              << std::endl;

#if !defined(NDEBUG) && (defined(HAS_CPLEX) || defined(HAS_SOPLEX))
    verify(mdp, value_table, initial_state);
#endif
}

} // namespace probfd::pdbs
