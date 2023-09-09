#ifndef PROBFD_HEURISTICS_PDBS_DOMINANCE_PRUNING_H
#define PROBFD_HEURISTICS_PDBS_DOMINANCE_PRUNING_H

#include "probfd/heuristics/pdbs/types.h"

#include "downward/utils/logging.h"

#include <vector>

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProbabilityAwarePatternDatabase;

/*
  Clique superset dominates clique subset iff for every pattern
  p_subset in subset there is a pattern p_superset in superset where
  p_superset is a superset of p_subset.
*/
extern void prune_dominated_cliques(
    PPDBCollection& pdbs,
    std::vector<PatternSubCollection>& pattern_cliques,
    int num_variables,
    double max_time,
    utils::LogProxy log);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_DOMINANCE_PRUNING_H
