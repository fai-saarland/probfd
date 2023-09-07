#ifndef PROBFD_HEURISTICS_ABSTRACTIONS_DISTANCES_H
#define PROBFD_HEURISTICS_ABSTRACTIONS_DISTANCES_H

#include "probfd/heuristics/abstractions/types.h"

#include "probfd/value_type.h"

#include <span>

namespace probfd::heuristics::abstractions {

/**
 * @brief Computes the optimal value function of the abstraction, complete up to
 * forward reachability from the initial state.
 */
template <typename Action>
void compute_value_table(
    AbstractMDP<Action>& mdp,
    AbstractStateIndex initial_state,
    const AbstractionEvaluator& heuristic,
    std::span<value_t> value_table,
    double max_time = std::numeric_limits<double>::infinity());

} // namespace probfd::heuristics::abstractions

#endif // PROBFD_HEURISTICS_ABSTRACTIONS_DISTANCES_H
