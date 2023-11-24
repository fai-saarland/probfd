#ifndef PROBFD_CARTESIAN_UTILS_H
#define PROBFD_CARTESIAN_UTILS_H

#include "probfd/task_proxy.h"

#include "probfd/cartesian_abstractions/types.h"

#include "downward/cartesian_abstractions/cartesian_set.h"
#include "downward/cartesian_abstractions/utils.h"

#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

namespace additive_heuristic {
class AdditiveHeuristic;
}

namespace probfd {
class ProbabilisticTask;

namespace cartesian_abstractions {

// Create the Cartesian set that corresponds to the given preconditions or
// goals.
template <typename ConditionsProxy>
CartesianSet get_cartesian_set(
    const std::vector<int>& domain_sizes,
    const ConditionsProxy& conditions)
{
    CartesianSet cartesian_set(domain_sizes);
    for (FactProxy condition : conditions) {
        cartesian_set.set_single_value(
            condition.get_variable().get_id(),
            condition.get_value());
    }
    return cartesian_set;
}

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

} // namespace cartesian_abstractions
} // namespace probfd

#endif // PROBFD_CARTESIAN_UTILS_H
