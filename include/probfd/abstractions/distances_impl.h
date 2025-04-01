#ifndef GUARD_INCLUDE_PROBFD_ABSTRACTIONS_DISTANCES_H
#error "This file should only be included from distances.h"
#endif

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/heuristic.h"

#include "downward/utils/countdown_timer.h"

#if !defined(NDEBUG) && (defined(HAS_CPLEX) || defined(HAS_SOPLEX))
#include "downward/lp/lp_solver.h"
#include "probfd/abstractions/verification.h"
#endif

namespace probfd {

template <typename State, typename Action>
void compute_value_table(
    MDP<State, Action>& mdp,
    ParamType<State> initial_state,
    const Heuristic<State>& heuristic,
    std::span<value_t> value_table,
    double max_time)
{
    using namespace algorithms::ta_topological_vi;

    utils::CountdownTimer timer(max_time);

    TATopologicalValueIteration<State, Action> vi(10e-5, value_table.size());
    vi.solve(
        mdp,
        heuristic,
        initial_state,
        value_table,
        timer.get_remaining_time());

#if !defined(NDEBUG) && (defined(HAS_CPLEX) || defined(HAS_SOPLEX))
    lp::LPSolverType lp_solver_type;
#if defined(HAS_CPLEX)
    lp_solver_type = lp::LPSolverType::CPLEX;
#else
    // lp_solver_type = lp::LPSolverType::SOPLEX;
#endif
    verify(mdp, value_table, lp_solver_type);
#endif
}

} // namespace probfd
