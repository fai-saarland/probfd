#ifndef PROBFD_TERMINATION_COST_FUNCTION_H
#define PROBFD_TERMINATION_COST_FUNCTION_H

#include "probfd/type_traits.h"
#include "probfd/value_type.h"

namespace probfd {

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
        return is_goal ? -1.0_vt : 0.0_vt;
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
     * @brief Returns the cost to terminate in a given state.
     *
     * @see TerminationInfo
     */
    virtual value_t get_termination_cost(ParamType<State> state) = 0;
};

template <typename State>
class SimpleTerminationCostFunction : public TerminationCostFunction<State> {
public:
    /**
     * @brief Get the termination cost info of the input state.
     */
    value_t get_termination_cost(ParamType<State> state) final
    {
        return is_goal(state) ? get_goal_termination_cost()
                              : get_non_goal_termination_cost();
    }

    virtual bool is_goal(ParamType<State> state) const = 0;

    virtual value_t get_goal_termination_cost() const = 0;

    virtual value_t get_non_goal_termination_cost() const = 0;
};

} // namespace probfd

#endif