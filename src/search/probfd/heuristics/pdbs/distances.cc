#include "probfd/heuristics/abstractions/distances-impl.h"

#include "probfd/heuristics/pdbs/projection_operator.h"

namespace probfd::heuristics::abstractions {

template void compute_value_table<const pdbs::ProjectionOperator*>(
    AbstractMDP<const pdbs::ProjectionOperator*>& state_space,
    AbstractStateIndex initial_state,
    const AbstractionEvaluator& heuristic,
    std::span<value_t> value_table,
    double max_time);

} // namespace probfd::heuristics::abstractions
