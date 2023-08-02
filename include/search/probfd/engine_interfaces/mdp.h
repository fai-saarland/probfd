#ifndef PROBFD_ENGINE_INTERFACES_MDP_H
#define PROBFD_ENGINE_INTERFACES_MDP_H

#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/state_space.h"

namespace probfd {

/// This namespace contains interfaces related to search engines.
namespace engine_interfaces {

/**
 * @brief Wraps a state space and a cost function for this state space.
 *
 * @tparam State - The state type of the MDP.
 * @tparam Action - The action type of the MDP.
 */
template <typename State, typename Action>
struct MDP {
    StateSpace<State, Action>& state_space;
    CostFunction<State, Action>& cost_function;

    /**
     * @brief Get the state ID for a given state.
     */
    StateID get_state_id(param_type<State> state)
    {
        return state_space.get_state_id(state);
    }

    /**
     * @brief Get the state mapped to a given state ID.
     */
    State get_state(StateID state_id)
    {
        return state_space.get_state(state_id);
    }

    /**
     * @brief Generates the applicable actions of the state.
     */
    void generate_applicable_actions(StateID state, std::vector<Action>& result)
    {
        return state_space.generate_applicable_actions(state, result);
    }

    /**
     * @brief Generates the successor distribution for a given state and action.
     */
    void generate_action_transitions(
        StateID state,
        param_type<Action> action,
        Distribution<StateID>& result)
    {
        return state_space.generate_action_transitions(state, action, result);
    }

    /**
     * @brief Generates all applicable actions and their corresponding successor
     * distributions for a given state.
     */
    void generate_all_transitions(
        StateID state,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors)
    {
        return state_space.generate_all_transitions(state, aops, successors);
    }

    /**
     * @brief Returns the cost to terminate in a given state and checks whether
     * a state is a goal
     *
     * @see TerminationInfo
     */
    TerminationInfo get_termination_info(param_type<State> state)
    {
        return cost_function.get_termination_info(state);
    }

    /**
     * @brief Gets the action cost of a state-action.
     */
    value_t get_action_cost(param_type<Action> action)
    {
        return cost_function.get_action_cost(action);
    }
};

// Decution guide for aggregate initialization
template <typename State, typename Action>
MDP(State, Action) -> MDP<State, Action>;

} // namespace engine_interfaces
} // namespace probfd

#endif