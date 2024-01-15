#ifndef PROBFD_PDBS_UTILS_H
#define PROBFD_PDBS_UTILS_H

#include "probfd/pdbs/types.h"

#include <iosfwd>
#include <vector>

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
class ProbabilisticTaskProxy;

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
} // namespace probfd

#endif // PROBFD_PDBS_UTILS_H
