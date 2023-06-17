#ifndef PROBFD_HEURISTICS_CARTESIAN_TYPES_H
#define PROBFD_HEURISTICS_CARTESIAN_TYPES_H

#include "downward/cegar/types.h"

#include <limits>
#include <memory>
#include <unordered_set>
#include <vector>

namespace cegar {
class CartesianSet;
class CartesianHeuristicFunction;
class RefinementHierarchy;
}

namespace probfd {
template <typename, typename>
class PartialPolicy;

namespace heuristics {
namespace cartesian {

struct ProbabilisticTransition;

// Classed reused from classical implementation
using CartesianSet = cegar::CartesianSet;
using RefinementHierarchy = cegar::RefinementHierarchy;

// Typedefs reused from classical implementation
using Goals = cegar::Goals;
using NodeID = cegar::NodeID;

// Typdefs adapted from classical implementation
class AbstractState;
using AbstractStates = std::vector<std::unique_ptr<AbstractState>>;

using Loops = std::vector<int>;
using ProbabilisticTransitions = std::vector<ProbabilisticTransition>;

static constexpr int UNDEFINED = -1;

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
