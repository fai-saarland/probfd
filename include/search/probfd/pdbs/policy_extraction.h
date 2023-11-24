#ifndef PROBFD_PDBS_POLICY_EXTRACTION_H
#define PROBFD_PDBS_POLICY_EXTRACTION_H

#include "probfd/pdbs/types.h"

#include "probfd/value_type.h"

#include <memory>
#include <span>

namespace utils {
class RandomNumberGenerator;
}

namespace probfd::pdbs {

class ProjectionStateSpace;

/**
 * @brief Extract an abstract optimal policy from the value table.
 *
 * Tie-breaking is performed randomly using the input RNG. If the \p
 * wildcard option is specified, a wildcard policy will be returned, i.e., a
 * policy that assigns multiple equivalent operators to a abstract state.
 */
[[nodiscard]] std::unique_ptr<ProjectionMultiPolicy>
compute_optimal_projection_policy(
    ProjectionStateSpace& state_space,
    std::span<const value_t> value_table,
    StateRank initial_state,
    utils::RandomNumberGenerator& rng,
    bool wildcard);

/**
 * @brief Extracts an abstract greedy policy from the value table, which may not
 * be optimal if traps are existent.
 *
 * Tie-breaking is performed randomly using the input RNG. If the \p
 * wildcard option is specified, a wildcard policy will be returned, i.e., a
 * policy that assigns multiple equivalent operators to a abstract state.
 */
[[nodiscard]] std::unique_ptr<ProjectionMultiPolicy>
compute_greedy_projection_policy(
    ProjectionStateSpace& state_space,
    std::span<const value_t> value_table,
    StateRank initial_state,
    utils::RandomNumberGenerator& rng,
    bool wildcard);

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_POLICY_EXTRACTION_H
