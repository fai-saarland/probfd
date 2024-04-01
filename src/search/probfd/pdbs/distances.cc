#include "probfd/pdbs/distances.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/evaluator.h"

#include "downward/utils/countdown_timer.h"

#include <ranges>

#if !defined(NDEBUG) && (defined(HAS_CPLEX) || defined(HAS_SOPLEX))
#include "downward/lp/lp_solver.h"
#include "probfd/pdbs/verification.h"
#endif

namespace probfd::pdbs {

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

} // namespace probfd::pdbs
