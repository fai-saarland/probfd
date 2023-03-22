#ifndef PROBFD_ENGINE_INTERFACES_STATE_SPACE_H
#define PROBFD_ENGINE_INTERFACES_STATE_SPACE_H

#include "probfd/distribution.h"
#include "probfd/type_traits.h"
#include "probfd/types.h"


#include <vector>

namespace probfd {
namespace engine_interfaces {

template <typename State, typename Action>
class StateSpace {
public:
    virtual ~StateSpace() = default;

    /**
     * @brief Get the state id for a given state.
     */
    virtual StateID get_state_id(param_type<State> state) = 0;

    /**
     * @brief Get the state for a given state id.
     */
    virtual State get_state(StateID state_id) = 0;

    /**
     * @brief Get the action id of a given state action.
     *
     * @param state_id - The ID of the state in which the action is applicable.
     * @param action - The action.
     * @return ActionID - An ID representing this state action.
     */
    virtual ActionID
    get_action_id(StateID state_id, param_type<Action> action) = 0;

    /**
     * @brief Get the action for a given state and action ID.
     *
     * @param state_id - The ID of the state in which the represented action is
     * applicable.
     * @param action_id - The action ID.
     * @returns Action - The represented action.
     */
    virtual Action get_action(StateID state_id, ActionID action_id) = 0;

    /**
     * Generates all applicable actions of the state \p state and outputs them
     * in \p result.
     */
    virtual void
    generate_applicable_actions(StateID state, std::vector<Action>& result) = 0;

    virtual void generate_action_transitions(
        StateID state,
        param_type<Action> action,
        Distribution<StateID>& result) = 0;

    virtual void generate_all_transitions(
        StateID state,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors) = 0;
};

} // namespace engine_interfaces
} // namespace probfd

#endif