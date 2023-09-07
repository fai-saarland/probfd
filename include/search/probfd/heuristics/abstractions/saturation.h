#ifndef PROBFD_HEURISTICS_ABSTRACTIONS_SATURATION_H
#define PROBFD_HEURISTICS_ABSTRACTIONS_SATURATION_H

#include "probfd/heuristics/abstractions/types.h"

#include "probfd/value_type.h"

#include <span>

namespace probfd::heuristics::abstractions {

template <typename Action>
void compute_saturated_costs(
    AbstractMDP<Action>& mdp,
    std::span<const value_t> value_table,
    std::span<value_t> saturated_costs);

} // namespace probfd::heuristics::abstractions

#endif // PROBFD_HEURISTICS_ABSTRACTIONS_SATURATION_H
