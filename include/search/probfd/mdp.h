#ifndef PROBFD_MDP_H
#define PROBFD_MDP_H

#include "probfd/cost_function.h"
#include "probfd/state_space.h"

namespace probfd {

/**
 * @brief Basic interface for MDPs.
 */
template <typename State, typename Action>
class MDP
    : public StateSpace<State, Action>
    , public CostFunction<State, Action> {
    /**
     * @brief Prints statistics, e.g. the number of queries made to the
     * interface.
     */
    virtual void print_statistics() const {}
};

/**
 * @brief Basic interface for MDPs.
 */
template <typename State, typename Action>
class SimpleMDP : public MDP<State, Action> {
public:
    TerminationInfo get_termination_info(param_type<State> state) override final
    {
        const bool goal = is_goal(state);
        return TerminationInfo(
            goal,
            goal ? get_goal_termination_cost()
                 : get_non_goal_termination_cost());
    }

    virtual bool is_goal(param_type<State> state) const = 0;
    virtual value_t get_goal_termination_cost() const = 0;
    virtual value_t get_non_goal_termination_cost() const = 0;
};

/**
 * @brief Composes a state space and a cost function to an MDP.
 *
 * @tparam State - The state type of the MDP.
 * @tparam Action - The action type of the MDP.
 */
template <typename State, typename Action>
struct CompositeMDP : public MDP<State, Action> {
    StateSpace<State, Action>& state_space;
    CostFunction<State, Action>& cost_function;

    /**
     * @brief Get the state ID for a given state.
     */
    StateID get_state_id(param_type<State> state) override final
    {
        return state_space.get_state_id(state);
    }

    /**
     * @brief Get the state mapped to a given state ID.
     */
    State get_state(StateID state_id) override final
    {
        return state_space.get_state(state_id);
    }

    /**
     * @brief Generates the applicable actions of the state.
     */
    void generate_applicable_actions(StateID state, std::vector<Action>& result)
        override final
    {
        return state_space.generate_applicable_actions(state, result);
    }

    /**
     * @brief Generates the successor distribution for a given state and action.
     */
    void generate_action_transitions(
        StateID state,
        param_type<Action> action,
        Distribution<StateID>& result) override final
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
        std::vector<Distribution<StateID>>& successors) override final
    {
        return state_space.generate_all_transitions(state, aops, successors);
    }

    /**
     * @brief Returns the cost to terminate in a given state and checks whether
     * a state is a goal
     *
     * @see TerminationInfo
     */
    TerminationInfo get_termination_info(param_type<State> state) override final
    {
        return cost_function.get_termination_info(state);
    }

    /**
     * @brief Gets the action cost of a state-action.
     */
    value_t get_action_cost(param_type<Action> action) override final
    {
        return cost_function.get_action_cost(action);
    }
};

// Decution guide for aggregate initialization
template <typename State, typename Action>
CompositeMDP(State, Action) -> CompositeMDP<State, Action>;

} // namespace probfd

#endif