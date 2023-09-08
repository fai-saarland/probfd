#include "probfd/maxprob_cost_function.h"

#include "probfd/task_utils/task_properties.h"

namespace probfd {

MaxProbCostFunction::MaxProbCostFunction(ProbabilisticTaskProxy task_proxy)
    : task_proxy(task_proxy)
{
}

bool MaxProbCostFunction::is_goal(param_type<State> state) const
{
    return ::task_properties::is_goal_state(task_proxy, state);
}

value_t MaxProbCostFunction::get_non_goal_termination_cost() const
{
    return 1_vt;
}

value_t MaxProbCostFunction::get_action_cost(OperatorID)
{
    return 0_vt;
}

} // namespace probfd