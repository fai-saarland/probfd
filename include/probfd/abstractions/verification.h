#ifndef PROBFD_PDBS_VERIFICATION_H
#define PROBFD_PDBS_VERIFICATION_H

#include "probfd/value_type.h"

#include <span>

namespace lp {
enum class LPSolverType;
}

namespace probfd {
template <typename, typename>
class MDP;
}

namespace probfd {

/**
 * @brief Computes the optimal value function of the abstraction, complete up to
 * forward reachability from the initial state.
 */
template <typename State, typename Action>
void verify(
    MDP<State, Action>& mdp,
    std::span<const value_t> value_table,
    lp::LPSolverType type);

} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ABSTRACTIONS_VERIFICATION_H
#include "probfd/abstractions/verification_impl.h"
#undef GUARD_INCLUDE_PROBFD_ABSTRACTIONS_VERIFICATION_H

#endif
