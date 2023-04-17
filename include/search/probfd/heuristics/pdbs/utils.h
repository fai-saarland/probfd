#ifndef PROBFD_HEURISTICS_PDBS_UTILS_H
#define PROBFD_HEURISTICS_PDBS_UTILS_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_proxy.h"

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

Pattern extended_pattern(const Pattern& pattern, int add_var);

std::vector<int> get_goals_in_random_order(
    const ProbabilisticTaskProxy& task_proxy,
    utils::RandomNumberGenerator& rng);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TYPES_H__