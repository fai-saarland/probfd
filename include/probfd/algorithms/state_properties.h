#ifndef PROBFD_ALGORITHMS_STATE_PROPERTIES_H
#define PROBFD_ALGORITHMS_STATE_PROPERTIES_H

#include "probfd/state_id.h"
#include "probfd/value_type.h"

#include <optional>

// Forward Declarations
namespace probfd {
struct Interval;
}

namespace probfd::algorithms {

/**
 * @brief Interface providing access to various state properties during
 * heuristic search.
 *
 * @todo At the moment, the accessible properties are limited to state values.
 */
class StateProperties {
public:
    virtual ~StateProperties() = default;

    /**
     * @brief Looks up the current lower bound for the optimal value of a state.
     */
    virtual value_t lookup_value(StateID state_id) = 0;

    /**
     * @brief Looks up the bounding interval for the optimal value of a state.
     */
    virtual Interval lookup_bounds(StateID state_id) = 0;
};

} // namespace probfd::algorithms

#endif // PROBFD_ALGORITHMS_STATE_PROPERTIES_H