#ifndef PROBFD_ABSTRACTIONS_DISTANCES_H
#define PROBFD_ABSTRACTIONS_DISTANCES_H

#include "probfd/pdbs/types.h"

#include "probfd/type_traits.h"
#include "probfd/value_type.h"

#include <limits>
#include <span>

// Forward Declarations
namespace probfd {
template <typename>
class Heuristic;
template <typename, typename>
class MDP;
} // namespace probfd

namespace probfd {

/**
 * @brief Computes the optimal value function of the abstraction, complete up to
 * forward reachability from the initial state.
 */
template <typename State, typename Action>
void compute_value_table(
    MDP<State, Action>& mdp,
    ParamType<State> initial_state,
    const Heuristic<State>& heuristic,
    std::span<value_t> value_table,
    double max_time = std::numeric_limits<double>::infinity(),
    value_t epsilon = 10e-5);

} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ABSTRACTIONS_DISTANCES_H
#include "probfd/abstractions/distances_impl.h"
#undef GUARD_INCLUDE_PROBFD_ABSTRACTIONS_DISTANCES_H

#endif
