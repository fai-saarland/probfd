#ifndef PROBFD_PDBS_DISTANCES_H
#define PROBFD_PDBS_DISTANCES_H

#include "probfd/pdbs/types.h"

#include "probfd/value_type.h"

#include <span>

// Forward Declarations
namespace probfd {
template <typename>
class Evaluator;
}

namespace probfd::pdbs {
class ProjectionStateSpace;
}

namespace probfd::pdbs {

/**
 * @brief Computes the optimal value function of the abstraction, complete up to
 * forward reachability from the initial state.
 */
void compute_value_table(
    ProjectionStateSpace& state_space,
    StateRank initial_state,
    const Evaluator<StateRank>& heuristic,
    std::span<value_t> value_table,
    double max_time);

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_DISTANCES_H
