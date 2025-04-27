#ifndef CEGAR_TYPES_H
#define CEGAR_TYPES_H

#include "downward/abstract_task.h"

#include <limits>
#include <memory>
#include <unordered_set>
#include <vector>

namespace downward {
class StateMapping;
class InverseOperatorMapping;
}

namespace downward::cartesian_abstractions {
class AbstractState;
struct Transition;

using AbstractStates = std::vector<std::unique_ptr<AbstractState>>;
using Goals = std::unordered_set<int>;
using NodeID = int;
using Loops = std::vector<int>;
using Transitions = std::vector<Transition>;

const int UNDEFINED = -1;

// Positive infinity. The name "INFINITY" is taken by an ISO C99 macro.
const int INF = std::numeric_limits<int>::max();

using SharedTasks = std::vector<std::tuple<
    SharedAbstractTask,
    std::shared_ptr<StateMapping>,
    std::shared_ptr<InverseOperatorMapping>>>;

} // namespace cartesian_abstractions

#endif
