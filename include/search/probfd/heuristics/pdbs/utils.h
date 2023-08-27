#ifndef PROBFD_HEURISTICS_PDBS_UTILS_H
#define PROBFD_HEURISTICS_PDBS_UTILS_H

#include "probfd/heuristics/pdbs/evaluators.h"
#include "probfd/heuristics/pdbs/state_rank.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_proxy.h"

#include "probfd/value_type.h"

#include <memory>
#include <vector>

namespace pdbs {
class PatternCollectionGenerator;
class PatternCollectionInformation;
} // namespace pdbs

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProjectionStateSpace;

Pattern extended_pattern(const Pattern& pattern, int add_var);

std::vector<int> get_goals_in_random_order(
    const ProbabilisticTaskProxy& task_proxy,
    utils::RandomNumberGenerator& rng);

std::ostream& operator<<(std::ostream& out, const Pattern& pattern);

/**
 * @brief Computes the optimal state value table of the projection. The state
 * values for states that are unreachable remain unchanged.
 */
void compute_value_table(
    ProjectionStateSpace& state_space,
    const StateRankEvaluator& heuristic,
    StateRank initial_state,
    double max_time,
    std::vector<value_t>& value_table);

/**
 * @brief Computes the saturated costs of a projection.
 */
void compute_saturated_costs(
    ProjectionStateSpace& state_space,
    const std::vector<value_t>& value_table,
    std::vector<value_t>& saturated_costs);

/**
 * @brief Extract an abstract optimal policy for this projection from the given
 * value table.
 *
 * Tie-breaking is performed randomly using the input RNG. If the wildcard
 * option is specified, a wildcard policy will be returned, i.e., a policy that
 * assigns multiple equivalent operators to a abstract state.
 */
[[nodiscard]] std::unique_ptr<ProjectionMultiPolicy>
compute_optimal_projection_policy(
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    const std::vector<value_t>& value_table,
    utils::RandomNumberGenerator& rng,
    bool wildcard);

/**
 * @brief Extract a greedy policy for this projection from the given value
 * table, which may not be optimal if traps are existent.
 *
 * Tie-breaking is performed randomly using the input RNG. If the wildcard
 * option is specified, a wildcard policy will be returned, i.e., a policy that
 * assigns multiple equivalent operators to a abstract state.
 */
[[nodiscard]] std::unique_ptr<ProjectionMultiPolicy>
compute_greedy_projection_policy(
    ProjectionStateSpace& state_space,
    StateRank initial_state,
    const std::vector<value_t>& value_table,
    utils::RandomNumberGenerator& rng,
    bool wildcard);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TYPES_H__