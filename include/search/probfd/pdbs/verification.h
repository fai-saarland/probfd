#ifndef PROBFD_PDBS_VERIFICATION_H
#define PROBFD_PDBS_VERIFICATION_H

#include "probfd/value_type.h"

#include <span>

namespace lp {
enum class LPSolverType;
}

namespace probfd::pdbs {
class ProjectionStateSpace;
}

namespace probfd::pdbs {

/**
 * @brief Computes the optimal value function of the abstraction, complete up to
 * forward reachability from the initial state.
 */
void verify(
    ProjectionStateSpace& mdp,
    std::span<const value_t> value_table,
    lp::LPSolverType type);

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_VERIFICATION_H
