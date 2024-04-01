#include "probfd/pdbs/distances.h"

#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/evaluator.h"

#include <ranges>

#include "downward/lp/lp_solver.h"

#include <cmath>
namespace probfd::pdbs {

void verify(
    ProjectionStateSpace& mdp,
    std::span<const value_t> value_table,
    lp::LPSolverType type)
{
    lp::LPSolver solver(type);
    const double inf = solver.get_infinity();
    const value_t term_cost = mdp.get_non_goal_termination_cost();

    named_vector::NamedVector<lp::LPVariable> variables;

    const int num_states = static_cast<int>(value_table.size());

    for (int i = 0; i != num_states; ++i) {
        variables.emplace_back(0_vt, std::min(term_cost, inf), 0_vt);
    }

    named_vector::NamedVector<lp::LPConstraint> constraints;

    for (int i = 0; i != num_states; ++i) {
        const auto value = value_table[i];
        variables[i].objective_coefficient =
            value != INFINITE_VALUE && !std::isnan(value) ? 1_vt : 0_vt;

        if (mdp.is_goal(i)) {
            auto& g = constraints.emplace_back(0_vt, 0_vt);
            g.insert(i, 1_vt);
        }

        // Generate operators...
        std::vector<const ProjectionOperator*> aops;
        mdp.generate_applicable_actions(i, aops);

        // Push successors
        for (const ProjectionOperator* op : aops) {
            const value_t cost = mdp.get_action_cost(op);

            Distribution<StateID> successor_dist;
            mdp.generate_action_transitions(i, op, successor_dist);

            if (successor_dist.is_dirac(i)) {
                continue;
            }

            auto& constr = constraints.emplace_back(-inf, cost);

            value_t non_loop_prob = 0_vt;
            for (const auto& [succ, prob] : successor_dist) {
                if (succ != static_cast<size_t>(i)) {
                    non_loop_prob += prob;
                    constr.insert(succ.id, -prob);
                }
            }

            constr.insert(i, non_loop_prob);
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
        const auto value = value_table[s];
        if (value != INFINITE_VALUE && !std::isnan(value)) {
            assert(is_approx_equal(value, solution[s], 0.001));
        }
    }
}

} // namespace probfd::pdbs
