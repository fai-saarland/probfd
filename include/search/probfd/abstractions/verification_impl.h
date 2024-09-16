#ifndef GUARD_INCLUDE_PROBFD_ABSTRACTIONS_VERIFICATION_H
#error "This file should only be included from verification.h"
#endif

#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/evaluator.h"

#include "downward/lp/lp_solver.h"

#include <cmath>
#include <ranges>

namespace probfd {

template <typename State, typename Action>
void verify(
    MDP<State, Action>& mdp,
    std::span<const value_t> value_table,
    lp::LPSolverType type)
{
    lp::LPSolver solver(type);
    const double inf = solver.get_infinity();

    named_vector::NamedVector<lp::LPVariable> variables;
    named_vector::NamedVector<lp::LPConstraint> constraints;

    const std::size_t num_states = value_table.size();

    for (std::size_t i = 0; i != num_states; ++i) {
        const State state = mdp.get_state(i);
        const auto term_info = mdp.get_termination_info(state);
        const value_t term_cost = term_info.get_cost();

        const auto value = value_table[i];

        variables.emplace_back(
            -inf,
            std::min(term_cost, inf),
            value != INFINITE_VALUE && !std::isnan(value) ? 1_vt : 0_vt);

        // Generate operators...
        std::vector<Action> aops;
        mdp.generate_applicable_actions(i, aops);

        // Push successors
        for (const Action& op : aops) {
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

    for (StateID s = 0; s.id != num_states; ++s.id) {
        const auto value = value_table[s];
        if (value != INFINITE_VALUE && !std::isnan(value)) {
            assert(is_approx_equal(value, solution[s], 0.001));
        }
    }
}

} // namespace probfd
