#ifndef PROBFD_PDBS_SATURATION_H
#define PROBFD_PDBS_SATURATION_H

#include "probfd/value_type.h"

#include <span>

// Forward Declarations
namespace probfd::pdbs {
class ProjectionStateSpace;
}

namespace probfd::pdbs {

void compute_saturated_costs(
    ProjectionStateSpace& state_space,
    std::span<const value_t> value_table,
    std::span<value_t> saturated_costs);

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_SATURATION_H
