#ifndef MDPS_ENGINE_INTERFACES_STATE_ID_MAP_H
#define MDPS_ENGINE_INTERFACES_STATE_ID_MAP_H

#include "probfd/types.h"

namespace probfd {
namespace engine_interfaces {

/**
 * @brief Specifies a mapping between states and integer IDs.
 *
 * @tparam State - The state type of the underlying MDP model.
 */
template <typename State>
class StateIDMap {
public:
    /**
     * @brief Get the state id for a given state.
     */
    StateID get_state_id(const State& state);

    /**
     * @brief Get the state for a given state id.
     */
    State get_state(const StateID& state_id);
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __STATE_ID_MAP_H__