#ifndef PROBFD_HEURISTICS_PDBS_SATURATION_H
#define PROBFD_HEURISTICS_PDBS_SATURATION_H

#include "probfd/value_type.h"

#include <span>

namespace probfd::heuristics::pdbs {

class ProjectionStateSpace;

void compute_saturated_costs(
    ProjectionStateSpace& state_space,
    std::span<const value_t> value_table,
    std::span<value_t> saturated_costs);
} // namespace probfd::heuristics::pdbs

#endif // PROBFD_HEURISTICS_PDBS_SATURATION_H
