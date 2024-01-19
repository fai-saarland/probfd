#ifndef PROBFD_CARTESIAN_DISTANCES_H
#define PROBFD_CARTESIAN_DISTANCES_H

#include "probfd/value_type.h"

#include <vector>

// Forward Declarations
namespace probfd::cartesian_abstractions {
class Abstraction;
class CartesianHeuristic;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

/**
 * @brief Calls topological value iteration to compute the complete optimal
 * value function (for states reachable from the initial state).
 */
std::vector<value_t>
compute_distances(Abstraction& abstraction, CartesianHeuristic& heuristic);

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_DISTANCES_H
