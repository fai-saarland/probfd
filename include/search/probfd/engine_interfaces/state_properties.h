#ifndef PROBFD_ENGINE_INTERFACES_STATE_PROPERTIES_H
#define PROBFD_ENGINE_INTERFACES_STATE_PROPERTIES_H

#include "probfd/interval.h"
#include "probfd/types.h"

#include <optional>

namespace probfd {

namespace engines::heuristic_search {
class StateFlags;
}

namespace engine_interfaces {

/**
 * @brief Interface providing access to various state properties during
 * heuristic search.
 */
class StateProperties {
public:
    virtual ~StateProperties() = default;

    /**
     * @brief Provides access to the property flags of a given state.
     *
     * @todo The StateFlags struct is an implementation detail and should not be
     * exposed. It would be cleaner to replace this method by several different
     * methods like is_dead_end(StateID), is_goal(StateID) and so on.
     */
    virtual const engines::heuristic_search::StateFlags&
    lookup_state_flags(StateID state_id) = 0;

    /**
     * @brief Looks up the current lower bound for the optimal value of a state.
     */
    virtual value_t lookup_value(StateID state_id) = 0;

    /**
     * @brief Looks up the bounding interval for the optimal value of a state.
     */
    virtual Interval lookup_bounds(StateID state_id) = 0;
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __HEURISTIC_SEARCH_CONNECTOR_H__