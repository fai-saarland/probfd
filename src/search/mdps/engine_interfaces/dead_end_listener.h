#ifndef MDPS_ENGINE_INTERACES_DEAD_END_LISTENER_H
#define MDPS_ENGINE_INTERACES_DEAD_END_LISTENER_H

#include "../types.h"

#include <deque>

namespace probabilistic {

/**
 * @brief Enumeration describing the dead end identification level.
 *
 * - Disabled : Disables dead-end identification.
 * - Policy : ?
 * - Visited : ?
 * - Complete : ?
 *
 */
enum class DeadEndIdentificationLevel {
    Disabled,
    Policy,
    Visited,
    Complete,
};

/**
 * @brief Function object doing what exactly? TODO
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class DeadEndListener {
public:
    /// Tags the default implementation of this template
    using is_default_implementation = std::true_type;

    bool operator()(const StateID&) { return false; }

    bool operator()(
        std::vector<StateID>::const_iterator,
        std::vector<StateID>::const_iterator)
    {
        return false;
    }
};

} // namespace probabilistic

#endif // __DEAD_END_LISTENER_H__