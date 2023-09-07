#ifndef PROBFD_HEURISTICS_PDBS_SATURATION_H
#define PROBFD_HEURISTICS_PDBS_SATURATION_H

#include "probfd/heuristics/abstractions/saturation.h"

namespace probfd::heuristics::pdbs {
class ProjectionOperator;
}

namespace probfd::heuristics::abstractions {

extern template void compute_saturated_costs<const pdbs::ProjectionOperator*>(
    AbstractMDP<const pdbs::ProjectionOperator*>& mdp,
    std::span<const value_t> value_table,
    std::span<value_t> saturated_costs);

} // namespace probfd::heuristics::abstractions

#endif // PROBFD_HEURISTICS_PDBS_SATURATION_H
