#ifndef PROBFD_HEURISTICS_PDBS_DISTANCES_H
#define PROBFD_HEURISTICS_PDBS_DISTANCES_H

#include "probfd/heuristics/abstractions/distances.h"

namespace probfd::heuristics::pdbs {
class ProjectionOperator;
}

namespace probfd::heuristics::abstraction {

extern template void compute_value_table<const pdbs::ProjectionOperator*>(
    ProjectionStateSpace& state_space,
    StateRank initial_state,
    const Evaluator<StateRank>& heuristic,
    std::span<value_t> value_table,
    double max_time);

} // namespace probfd::heuristics::abstraction

#endif // PROBFD_HEURISTICS_PDBS_DISTANCES_H
