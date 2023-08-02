#ifndef PROBFD_ENGINE_INTERFACES_COST_FUNCTION_H
#define PROBFD_ENGINE_INTERFACES_COST_FUNCTION_H

#include "probfd/type_traits.h"
#include "probfd/types.h"
#include "probfd/value_type.h"

namespace probfd {

/**
 * @brief Specifies the termination cost and goal status of a state.
 */
class TerminationInfo {
    bool is_goal_;
    value_t terminal_cost_;

public:
    TerminationInfo() = default;

    TerminationInfo(bool is_goal, value_t terminal_cost)
        : is_goal_(is_goal)
        , terminal_cost_(terminal_cost)
    {
    }

    /// Check if this state is a goal.
    bool is_goal_state() const { return is_goal_; }

    /// Obtains the cost paid upon termination in the state.
    value_t get_cost() const { return terminal_cost_; }
};

namespace engine_interfaces {

/**
 * @brief The interface specifying action and state termination costs, aswell as
 * the goal states of a state space.
 *
 * This interface communicates the action and termination costs and the goal
 * states of a state space to the MDP engines. Users must implement the public
 * methods
 * `get_termination_info(const State& state)` and
 * `get_action_cost(const Action& action)`.
 *
 * Example
 * =======
 *
 * ```
class MaxProbCostFunction : public CostFunction<State, OperatorID>
{
protected:
    TerminationInfo get_termination_info(const State& state) override
    {
        const bool is_goal = ...;

        // Terminate with -1 in goal states, 0 otherwise.
        return TerminationInfo(is_goal, is_goal ? -1.0_vt : 0.0_vt);
    }

    value_t get_action_cost(OperatorID) override
    {
        // Actions have no cost.
        return 0;
    }
};
 * ```
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class CostFunction {
public:
    virtual ~CostFunction() = default;

    /**
     * @brief Returns the cost to terminate in a given state and checks whether
     * a state is a goal
     *
     * @see TerminationInfo
     */
    virtual TerminationInfo get_termination_info(param_type<State> state) = 0;

    /**
     * @brief Gets the cost of an action.
     */
    virtual value_t get_action_cost(param_type<Action> action) = 0;
};

template <typename State, typename Action>
class SimpleCostFunction : public CostFunction<State, Action> {
public:
    virtual ~SimpleCostFunction() override = default;

    /**
     * @brief Get the termination cost info of the input state.
     */
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

} // namespace engine_interfaces
} // namespace probfd

#endif