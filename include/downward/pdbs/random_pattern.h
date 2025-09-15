#ifndef PDBS_RANDOM_PATTERN_H
#define PDBS_RANDOM_PATTERN_H

#include "downward/pdbs/types.h"

#include "downward/utils/timer.h"

#include <memory>

namespace downward {
class VariableSpace;
}

namespace downward::utils {
class LogProxy;
class RandomNumberGenerator;
} // namespace utils

namespace downward::pdbs {
/*
  This function computes a pattern for the given task. Starting with the given
  goal variable, the algorithm executes a random walk on the causal graph. In
  each iteration, it selects a random causal graph neighbor of the current
  variable (given via cg_neighbors). It terminates if no neighbor fits the
  pattern due to the size limit or if the time limit is reached.
*/
extern Pattern generate_random_pattern(
    int max_pdb_size,
    utils::Duration max_time,
    utils::LogProxy& log,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    const VariableSpace& variables,
    int goal_variable,
    std::vector<std::vector<int>>& cg_neighbors);

} // namespace pdbs

#endif
