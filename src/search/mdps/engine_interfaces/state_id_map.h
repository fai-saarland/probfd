#ifndef MDPS_ENGINE_INTERACES_STATE_ID_MAP_H
#define MDPS_ENGINE_INTERACES_STATE_ID_MAP_H

#include "../types.h"

namespace probabilistic {

/**
 * @brief Specified a mapping between states and integer IDs.
 *
 * @tparam State - The state type of the underlying MDP model.
 */
template <typename State>
struct StateIDMap {
    /**
     * Get the state id for a given state.
     */
    StateID get_state_id(const State& state);

    /**
     * Get the state for a given state id.
     */
    State get_state(const StateID& state_id);
};

} // namespace probabilistic

#endif // __STATE_ID_MAP_H__