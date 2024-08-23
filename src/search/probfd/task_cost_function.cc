#include "probfd/task_cost_function.h"

namespace probfd {

TaskCostFunction::TaskCostFunction(std::shared_ptr<ProbabilisticTask> task)
    : task_(std::move(task))
{
}

bool TaskCostFunction::is_goal(const State& state) const
{
    ProbabilisticTaskProxy proxy(*task_);
    return ::task_properties::is_goal_state(proxy, state);
}

value_t TaskCostFunction::get_non_goal_termination_cost() const
{
    return task_->get_non_goal_termination_cost();
}

value_t TaskCostFunction::get_action_cost(OperatorID action)
{
    return task_->get_operator_cost(action.get_index());
}

} // namespace probfd