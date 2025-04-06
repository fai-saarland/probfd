#ifndef GUARD_INCLUDE_PROBFD_ABSTRACTIONS_VERIFICATION_H
#error "This file should only be included from verification.h"
#endif

#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/heuristic.h"

#include "downward/lp/lp_solver.h"

#include <cmath>
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
}

} // namespace probfd
