#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_TYPES_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_TYPES_H

#include "downward/cartesian_abstractions/types.h"

#include <deque>
#include <memory>
#include <vector>

namespace downward::cartesian_abstractions {
class CartesianSet;
class CartesianHeuristicFunction;
class RefinementHierarchy;
} // namespace cartesian_abstractions

namespace probfd {
template <typename, typename>
class Policy;
}

namespace probfd::cartesian_abstractions {

struct TransitionOutcome;
struct ProbabilisticTransition;

// Classed reused from classical implementation
using CartesianSet = downward::cartesian_abstractions::CartesianSet;
using RefinementHierarchy = downward::cartesian_abstractions::RefinementHierarchy;

// Typedefs reused from classical implementation
using Goals = downward::cartesian_abstractions::Goals;
using NodeID = downward::cartesian_abstractions::NodeID;

// Typedefs adapted from classical implementation
class AbstractState;
using AbstractStates = std::vector<std::unique_ptr<AbstractState>>;

using Loops = std::vector<int>;
using ProbabilisticTransitions = std::vector<ProbabilisticTransition>;

using Trace = std::deque<TransitionOutcome>;

using Solution = Policy<int, const ProbabilisticTransition*>;

static constexpr int UNDEFINED = -1;

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_TYPES_H
