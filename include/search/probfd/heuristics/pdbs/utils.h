#ifndef PROBFD_HEURISTICS_PDBS_UTILS_H
#define PROBFD_HEURISTICS_PDBS_UTILS_H

#include "probfd/heuristics/pdbs/algorithm_interfaces.h"
#include "probfd/heuristics/pdbs/state_rank.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_proxy.h"

#include "probfd/value_type.h"

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
 * value for states that are unreachable remains unchanged.
 */
void compute_value_table(
    ProjectionStateSpace& state_space,
    const StateRankEvaluator& heuristic,
    StateRank initial_state,
    double max_time,
    std::vector<value_t>& value_table);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TYPES_H__