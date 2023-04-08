#ifndef PROBFD_ENGINE_INTERFACES_COST_FUNCTION_H
#define PROBFD_ENGINE_INTERFACES_COST_FUNCTION_H

#include "probfd/type_traits.h"
#include "probfd/types.h"
#include "probfd/value_type.h"

namespace probfd {

/**
 * @brief Contains cost paid upon termination in a state, and whether the state
 * is a goal.
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
 * @brief Interface specifying state termination costs, action costs and
 * goal states.
 *
 * This interface communicates the state and action costs and goal states to
 * the MDP engines. Users must implement the public methods
 * `get_termination_info(const State& state)` and
 * `get_action_cost(const State& state, const Action& action)`.
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

    value_t get_action_cost(const State& state, OperatorID) override
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
     * @brief Get the termination cost info of the input state.
     */
    virtual TerminationInfo get_termination_info(param_type<State> state) = 0;

    /**
     * @brief Get the action cost of the action when applied in a state.
     */
    virtual value_t
    get_action_cost(StateID state_id, param_type<Action> action) = 0;

    /**
     * @brief Prints statistics.
     */
    virtual void print_statistics() const {}
};

template <typename State, typename Action>
class StateIndependentCostFunction : public CostFunction<State, Action> {
public:
    virtual ~StateIndependentCostFunction() override = default;

    /**
     * @brief Get the action cost of the action when applied in a state.
     */
    value_t get_action_cost(StateID, param_type<Action> action) override final
    {
        return get_action_cost(action);
    }

    virtual value_t get_action_cost(param_type<Action> action) = 0;
};

template <typename State, typename Action>
class SimpleCostFunction : public StateIndependentCostFunction<State, Action> {
    value_t goal_termination;
    value_t non_goal_termination;

protected:
    SimpleCostFunction(value_t goal_termination, value_t non_goal_termination)
        : goal_termination(goal_termination)
        , non_goal_termination(non_goal_termination)
    {
    }

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
            goal ? goal_termination : non_goal_termination);
    }

    virtual bool is_goal(param_type<State> state) const = 0;

    value_t get_goal_termination_cost() const { return goal_termination; }

    value_t get_non_goal_termination_cost() const
    {
        return non_goal_termination;
    }
};

} // namespace engine_interfaces
} // namespace probfd

class State;
class OperatorID;

namespace probfd {

/// Type alias for cost functions for planning tasks.
using TaskCostFunction =
    engine_interfaces::SimpleCostFunction<State, OperatorID>;

} // namespace probfd

#endif