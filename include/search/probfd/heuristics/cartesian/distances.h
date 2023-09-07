#ifndef PROBFD_HEURISTICS_CARTESIAN_DISTANCES_H
#define PROBFD_HEURISTICS_CARTESIAN_DISTANCES_H

#include "probfd/heuristics/abstractions/distances.h"

#include <limits>
#include <span>

namespace probfd {
namespace heuristics {

namespace cartesian {
class ProbabilisticTransition;
}

namespace abstractions {

extern template void
compute_value_table<const cartesian::ProbabilisticTransition*>(
    AbstractMDP<const cartesian::ProbabilisticTransition*>& mdp,
    AbstractStateIndex initial_state,
    const AbstractionEvaluator& heuristic,
    std::span<value_t> value_table,
    double max_time);

} // namespace abstractions
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_CARTESIAN_DISTANCES_H
