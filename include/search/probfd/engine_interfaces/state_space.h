#ifndef PROBFD_ENGINE_INTERFACES_STATE_SPACE_H
#define PROBFD_ENGINE_INTERFACES_STATE_SPACE_H

#include "probfd/distribution.h"
#include "probfd/types.h"

#include <vector>

namespace probfd {
namespace engine_interfaces {

template <typename State, typename Action>
class StateSpace {
public:
    /**
     * @brief Get the state id for a given state.
     */
    StateID get_state_id(const State& state);

    /**
     * @brief Get the state for a given state id.
     */
    State get_state(StateID state_id);

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

    /**
     * Generates all applicable actions of the state \p state and outputs them
     * in \p result.
     */
    void
    generate_applicable_actions(StateID state, std::vector<Action>& result);

    void generate_action_transitions(
        StateID state,
        const Action& action,
        Distribution<StateID>& result);

    void generate_all_transitions(
        StateID state,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors);
};

} // namespace engine_interfaces
} // namespace probfd

#endif