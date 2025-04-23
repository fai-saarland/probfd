#include "probfd/task_cost_function.h"

#include "probfd/probabilistic_task.h"

using namespace downward;

namespace probfd {

TaskTerminationCostFunction::TaskTerminationCostFunction(
    std::shared_ptr<GoalFactList> goals,
    std::shared_ptr<TerminationCosts> costs)
    : goals_(std::move(goals))
    , costs_(std::move(costs))
{
}

bool TaskTerminationCostFunction::is_goal(const State& state) const
{
    return downward::task_properties::is_goal_state(*goals_, state);
}

value_t TaskTerminationCostFunction::get_goal_termination_cost() const
{
    return costs_->get_goal_termination_cost();
}

value_t TaskTerminationCostFunction::get_non_goal_termination_cost() const
{
    return costs_->get_non_goal_termination_cost();
}

TaskActionCostFunction::TaskActionCostFunction(
    std::shared_ptr<OperatorCostFunction<value_t>> op_cost_function)
    : op_cost_function_(std::move(op_cost_function))
{
}

value_t TaskActionCostFunction::get_action_cost(OperatorID action)
{
    return op_cost_function_->get_operator_cost(action.get_index());
}

} // namespace probfd