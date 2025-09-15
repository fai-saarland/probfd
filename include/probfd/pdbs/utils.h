#ifndef PROBFD_PDBS_UTILS_H
#define PROBFD_PDBS_UTILS_H

#include "probfd/pdbs/types.h"

#include "probfd/probabilistic_task.h"

#include <iosfwd>
#include <vector>

namespace downward {
class GoalFactList;
}

namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd::pdbs {
class ProjectionStateSpace;
struct ProbabilityAwarePatternDatabase;
} // namespace probfd::pdbs

namespace probfd::pdbs {

Pattern extended_pattern(const Pattern& pattern, int add_var);

std::vector<int> get_goals_in_random_order(
    const downward::GoalFactList& task,
    downward::utils::RandomNumberGenerator& rng);

/**
 * @brief Dump the PDB's projection as a dot graph to a specified path with
 * or without transition labels shown.
 */
void dump_graphviz(
    const ProbabilisticOperatorSpace& operators,
    ProjectionStateSpace& state_space,
    const ProbabilityAwarePatternDatabase& pdb,
    StateRank initial_state,
    std::ostream& out,
    bool transition_labels);

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_UTILS_H
