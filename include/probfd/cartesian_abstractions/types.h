#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_TYPES_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_TYPES_H

#include "downward/cartesian_abstractions/types.h"

#include <deque>
#include <memory>
#include <vector>

class StateMapping;
class InverseOperatorMapping;

namespace cartesian_abstractions {
class CartesianSet;
class CartesianHeuristicFunction;
class RefinementHierarchy;
} // namespace cartesian_abstractions

namespace probfd {
template <typename, typename>
class Policy;
class ProbabilisticTask;
} // namespace probfd

namespace probfd::cartesian_abstractions {

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

using SharedTasks = std::vector<std::tuple<
    std::shared_ptr<ProbabilisticTask>,
    std::shared_ptr<StateMapping>,
    std::shared_ptr<InverseOperatorMapping>>>;

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_TYPES_H
