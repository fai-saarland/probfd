#include "probfd/ssp_cost_function.h"

#include "probfd/task_utils/task_properties.h"

namespace probfd {

SSPCostFunction::SSPCostFunction(ProbabilisticTaskProxy task_proxy)
    : task_proxy(task_proxy)
{
}

bool SSPCostFunction::is_goal(param_type<State> state) const
{
    return ::task_properties::is_goal_state(task_proxy, state);
}

value_t SSPCostFunction::get_non_goal_termination_cost() const
{
    return INFINITE_VALUE;
}

value_t SSPCostFunction::get_action_cost(OperatorID op)
{
    return task_proxy.get_operators()[op].get_cost();
}

} // namespace probfd