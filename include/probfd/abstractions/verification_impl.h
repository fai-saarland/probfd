#ifndef GUARD_INCLUDE_PROBFD_ABSTRACTIONS_VERIFICATION_H
#error "This file should only be included from verification.h"
#endif

#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/heuristic.h"

#include "downward/lp/lp_solver.h"

#include <cmath>
#include <numeric>
#include <ranges>

namespace probfd {

template <typename State, typename Action>
void verify(
    MDP<State, Action>& mdp,
    std::span<const value_t> value_table,
    downward::lp::LPSolverType type)
{
    using namespace downward::lp;

    LPSolver solver(type);
    const double inf = solver.get_infinity();

    downward::named_vector::NamedVector<LPVariable> variables;
    downward::named_vector::NamedVector<LPConstraint> constraints;

    const std::size_t num_states = value_table.size();

    std::vector<int> unsolvable_states;

    for (std::size_t i = 0; i != num_states; ++i) {
        const State state = mdp.get_state(i);
        const auto term_info = mdp.get_termination_info(state);
        const value_t term_cost = term_info.get_cost();

        const auto value = value_table[i];

        if (value == INFINITE_VALUE) {
            unsolvable_states.push_back(i);
        }

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

            if (cost == INFINITE_VALUE) continue;

            SuccessorDistribution successor_dist;
            mdp.generate_action_transitions(i, op, successor_dist);

            if (successor_dist.non_source_successor_dist.empty()) {
                continue;
            }

            auto& constr = constraints.emplace_back(-inf, cost);

            for (const auto& [succ, prob] :
                 successor_dist.non_source_successor_dist) {
                constr.insert(succ.id, -prob);
            }

            constr.insert(i, successor_dist.non_source_probability);
        }
    }

    // Add extra state connected to unsolvable states.
    variables.emplace_back(-inf, inf, 0_vt);

    for (int state : unsolvable_states) {
        auto& constr = constraints.emplace_back(-inf, 0);
        constr.insert(state, -1.0);
        constr.insert(num_states, 1.0);
    }

    solver.load_problem(
        LinearProgram(
            LPObjectiveSense::MAXIMIZE,
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

    // Now check unsolvable states.
    // If the LP is unbounded, all unsolvable states are actually unsolvable.
    // If not, one of them has to be solvable.
    for (std::size_t i = 0; i != num_states; ++i) {
        solver.set_objective_coefficient(i, 0.0);
    }

    solver.set_objective_coefficient(num_states, 1.0);
    solver.solve();

    assert (!solver.has_optimal_solution() && solver.is_unbounded());
}

} // namespace probfd
