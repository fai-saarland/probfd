#ifndef PROBFD_COST_FUNCTION_H
#define PROBFD_COST_FUNCTION_H

#include "probfd/state_id.h"
#include "probfd/type_traits.h"
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

    static TerminationInfo from_goal(value_t value) { return {true, value}; }
    static TerminationInfo from_non_goal(value_t value)
    {
        return {false, value};
    }

    /// Check if this state is a goal.
    [[nodiscard]]
    bool is_goal_state() const
    {
        return is_goal_;
    }

    /// Obtains the cost paid upon termination in the state.
    [[nodiscard]]
    value_t get_cost() const
    {
        return terminal_cost_;
    }
};

/**
 * @brief The interface specifying action costs.
 *
 * This interface communicates the action costs of a state space to the MDP
 * algorithms.
 * Users must implement the public method
 * `value_t get_action_cost(ParamType<Action> action)`.
 *
 * Example
 * =======
 *
 * ```
class MaxProbActionCostFunction : public ActionCostFunction<OperatorID>
{
protected:
    value_t get_action_cost(OperatorID) override
    {
        // Actions have no cost.
        return 0;
    }
};
 * ```
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename Action>
class ActionCostFunction {
public:
    virtual ~ActionCostFunction() = default;
    /**
     * @brief Gets the cost of an action.
     */
    virtual value_t get_action_cost(ParamType<Action> action) = 0;
};

/**
 * @brief The interface specifying state termination costs of an MDP.
 *
 * This interface communicates the termination costs of a state space to the
 * MDP algorithms.
 * Users must implement the public method
 * `get_termination_info(ParamType<State> state)`.
 *
 * Example
 * =======
 *
 * ```
class MaxProbTerminationCostFunction : public TerminationCostFunction<State>
{
protected:
    TerminationInfo get_termination_info(const State& state) override
    {
        const bool is_goal = ...;

        // Terminate with -1 in goal states, 0 otherwise.
        return TerminationInfo(is_goal, is_goal ? -1.0_vt : 0.0_vt);
    }
};
 * ```
 *
 * @tparam State - The state type of the underlying MDP model.
 */
template <typename State>
class TerminationCostFunction {
public:
    virtual ~TerminationCostFunction() = default;

    /**
     * @brief Returns the cost to terminate in a given state and checks whether
     * a state is a goal
     *
     * @see TerminationInfo
     */
    virtual TerminationInfo get_termination_info(ParamType<State> state) = 0;

    /**
     * @brief Returns the cost to terminate in a given state.
     */
    value_t get_termination_cost(ParamType<State> state)
    {
        return get_termination_info(state).get_cost();
    }
};

template <typename State>
class SimpleTerminationCostFunction : public TerminationCostFunction<State> {
public:
    /**
     * @brief Get the termination cost info of the input state.
     */
    TerminationInfo get_termination_info(ParamType<State> state) final
    {
        return is_goal(state)
                   ? TerminationInfo::from_goal(get_goal_termination_cost())
                   : TerminationInfo::from_non_goal(
                         get_non_goal_termination_cost());
    }

    virtual bool is_goal(ParamType<State> state) const = 0;
    [[nodiscard]]
    virtual value_t get_goal_termination_cost() const = 0;
    virtual value_t get_non_goal_termination_cost() const = 0;
};

} // namespace probfd

#endif