#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_UTILS_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_UTILS_H

#include "probfd/cartesian_abstractions/types.h"

#include "downward/cartesian_abstractions/cartesian_set.h"

#include "downward/utils/hash.h"

#include "downward/state.h"

#include <memory>
#include <vector>

// Forward Declarations
namespace downward::additive_heuristic {
class AdditiveHeuristic;
}

namespace probfd {
class ProbabilisticTask;
} // namespace probfd

namespace probfd::cartesian_abstractions {

// Create the Cartesian set that corresponds to the given preconditions or
// goals.
template <typename ConditionsProxy>
CartesianSet get_cartesian_set(
    const std::vector<int>& domain_sizes,
    const ConditionsProxy& conditions)
{
    CartesianSet cartesian_set(domain_sizes);
    for (const auto [var, value] : conditions) {
        cartesian_set.set_single_value(var, value);
    }
    return cartesian_set;
}

extern std::unique_ptr<downward::additive_heuristic::AdditiveHeuristic>
create_additive_heuristic(const std::shared_ptr<ProbabilisticTask>& task);

/*
  The set of relaxed-reachable facts is the possibly-before set of facts that
  can be reached in the delete-relaxation before 'fact' is reached the first
  time, plus 'fact' itself.
*/
extern downward::utils::HashSet<downward::FactPair> get_relaxed_possible_before(
    const ProbabilisticTask& task,
    const downward::FactPair& fact);

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_UTILS_H
