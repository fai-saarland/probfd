#ifndef PROBFD_CARTESIAN_FLAW_H
#define PROBFD_CARTESIAN_FLAW_H

#include "probfd/cartesian_abstractions/types.h"

#include "downward/cartesian_abstractions/cartesian_set.h"

#include "downward/task_proxy.h"

#include <vector>

// Forward Declarations
namespace probfd::cartesian_abstractions {
class AbstractState;
struct Split;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

struct Flaw {
    // Last concrete and abstract state reached while tracing solution.
    State concrete_state;
    const AbstractState& current_abstract_state;
    // Hypothetical Cartesian set we would have liked to reach.
    CartesianSet desired_cartesian_set;

    Flaw(
        State&& concrete_state,
        const AbstractState& current_abstract_state,
        CartesianSet&& desired_cartesian_set);

    std::vector<Split> get_possible_splits() const;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_FLAW_H
