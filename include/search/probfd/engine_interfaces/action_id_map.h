#ifndef PROBFD_ENGINE_INTERFACES_ACTION_ID_MAP_H
#define PROBFD_ENGINE_INTERFACES_ACTION_ID_MAP_H

#include "probfd/types.h"

namespace probfd {

/// This namespace contains the interfaces used to communicate the state space
/// and the algorithm configurations to the search engines.
namespace engine_interfaces {

/**
 * @brief Specifies a mapping between state-action pairs and integer IDs.
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename Action>
class ActionIDMap {
public:
    /**
     * @brief Get the action id of a given state action.
     *
     * @param state_id - The ID of the state in which the action is applicable.
     * @param action - The action.
     * @return ActionID - An ID representing this state action.
     */
    ActionID get_action_id(StateID state_id, const Action& action);

    /**
     * @brief Get the action for a given state and action ID.
     *
     * @param state_id - The ID of the state in which the represented action is
     * applicable.
     * @param action_id - The action ID.
     * @returns Action - The represented action.
     */
    Action get_action(StateID state_id, ActionID action_id);
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __ACTION_ID_MAP_H__