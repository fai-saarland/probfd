#ifndef PROBFD_ACTION_COST_FUNCTION_H
#define PROBFD_ACTION_COST_FUNCTION_H

#include "probfd/type_traits.h"
#include "probfd/value_type.h"

namespace probfd {

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

} // namespace probfd

#endif