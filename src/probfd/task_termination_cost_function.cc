#include "probfd/task_termination_cost_function.h"

#include "probfd/termination_costs.h"

#include "downward/task_utils/task_properties.h"

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

} // namespace probfd