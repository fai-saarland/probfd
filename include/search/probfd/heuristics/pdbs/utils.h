#ifndef PROBFD_HEURISTICS_PDBS_UTILS_H
#define PROBFD_HEURISTICS_PDBS_UTILS_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_proxy.h"

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProjectionStateSpace;
class ProbabilityAwarePatternDatabase;

Pattern extended_pattern(const Pattern& pattern, int add_var);

std::vector<int> get_goals_in_random_order(
    ProbabilisticTaskProxy task_proxy,
    utils::RandomNumberGenerator& rng);

/**
 * @brief Dump the PDB's projection as a dot graph to a specified path with
 * or without transition labels shown.
 */
void dump_graphviz(
    ProbabilisticTaskProxy task_proxy,
    ProjectionStateSpace& state_space,
    const ProbabilityAwarePatternDatabase& pdb,
    StateRank initial_state,
    std::ostream& out,
    bool transition_labels);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TYPES_H__