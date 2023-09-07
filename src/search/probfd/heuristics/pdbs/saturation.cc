#include "probfd/heuristics/abstractions/saturation-impl.h"

#include "probfd/heuristics/pdbs/projection_operator.h"

namespace probfd::heuristics::abstractions {

template void compute_saturated_costs<const pdbs::ProjectionOperator*>(
    AbstractMDP<const pdbs::ProjectionOperator*>& mdp,
    std::span<const value_t> value_table,
    std::span<value_t> saturated_costs);

} // namespace probfd::heuristics::abstractions