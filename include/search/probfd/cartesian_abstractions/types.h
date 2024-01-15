#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_TYPES_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_TYPES_H

#include "downward/cartesian_abstractions/types.h"

#include <deque>
#include <limits>
#include <memory>
#include <unordered_set>
#include <vector>

namespace cartesian_abstractions {
class CartesianSet;
class CartesianHeuristicFunction;
class RefinementHierarchy;
} // namespace cartesian_abstractions

namespace probfd {
template <typename, typename>
class Policy;

namespace cartesian_abstractions {

struct TransitionOutcome;

struct ProbabilisticTransition;

// Classed reused from classical implementation
using CartesianSet = ::cartesian_abstractions::CartesianSet;
using RefinementHierarchy = ::cartesian_abstractions::RefinementHierarchy;

// Typedefs reused from classical implementation
using Goals = ::cartesian_abstractions::Goals;
using NodeID = ::cartesian_abstractions::NodeID;

// Typedefs adapted from classical implementation
class AbstractState;
using AbstractStates = std::vector<std::unique_ptr<AbstractState>>;

using Loops = std::vector<int>;
using ProbabilisticTransitions = std::vector<ProbabilisticTransition>;

using Trace = std::deque<TransitionOutcome>;

using Solution = Policy<int, const ProbabilisticTransition*>;

static constexpr int UNDEFINED = -1;

} // namespace cartesian_abstractions
} // namespace probfd

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_TYPES_H
