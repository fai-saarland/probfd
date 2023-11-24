#ifndef PROBFD_CARTESIAN_DISTANCES_H
#define PROBFD_CARTESIAN_DISTANCES_H

#include "probfd/value_type.h"

#include <vector>

namespace probfd {
namespace cartesian {

class Abstraction;
class CartesianHeuristic;

/**
 * @brief Calls topological value iteration to compute the complete optimal
 * value function (for states reachable from the initial state).
 */
std::vector<value_t>
compute_distances(Abstraction& abstraction, CartesianHeuristic& heuristic);

} // namespace cartesian
} // namespace probfd

#endif // PROBFD_CARTESIAN_DISTANCES_H
