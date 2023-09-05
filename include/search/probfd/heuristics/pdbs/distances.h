#ifndef PROBFD_HEURISTICS_PDBS_DISTANCES_H
#define PROBFD_HEURISTICS_PDBS_DISTANCES_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/value_type.h"

#include <span>

namespace probfd {
template <typename>
class Evaluator;

namespace heuristics::pdbs {

class ProjectionStateSpace;

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

} // namespace heuristics::pdbs
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_DISTANCES_H
