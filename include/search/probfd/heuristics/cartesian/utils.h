#ifndef PROBFD_HEURISTICS_CARTESIAN_UTILS_H
#define PROBFD_HEURISTICS_CARTESIAN_UTILS_H

#include "probfd/task_proxy.h"

#include "cegar/utils.h"

#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

namespace additive_heuristic {
class AdditiveHeuristic;
}

namespace probfd {
class ProbabilisticTask;

namespace heuristics {
namespace cartesian {

extern std::unique_ptr<additive_heuristic::AdditiveHeuristic>
create_additive_heuristic(const std::shared_ptr<ProbabilisticTask>& task);

/*
  The set of relaxed-reachable facts is the possibly-before set of facts that
  can be reached in the delete-relaxation before 'fact' is reached the first
  time, plus 'fact' itself.
*/
extern utils::HashSet<FactProxy> get_relaxed_possible_before(
    const ProbabilisticTaskProxy& task,
    const FactProxy& fact);

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
