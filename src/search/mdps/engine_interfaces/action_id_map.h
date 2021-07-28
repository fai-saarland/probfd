#pragma once

#include "../types.h"

namespace probabilistic {

/**
 * @brief Specifies a mapping between state actions and integer IDs.
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename Action>
struct ActionIDMap {
    /**
     * @brief Get the action id of a given state action.
     *
     * @param state_id - The ID of the state in which the action is applicable.
     * @param action - The action.
     * @return ActionID - An ID representing this state action.
     */
    ActionID get_action_id(const StateID& state_id, const Action& action);

    /**
     * @brief Get the action for a given state and action ID.
     *
     * @param state_id - The ID of the state in which the represented action is
     * applicable.
     * @param action_id - The action ID.
     * @returns Action - The represented action.
     */
    Action get_action(const StateID& state_id, const ActionID& action_id);
};

} // namespace probabilistic
