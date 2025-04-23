#include "downward/tasks/cost_adapted_task.h"

#include "downward/operator_cost.h"

#include "downward/task_utils/task_properties.h"

#include "downward/tasks/root_task.h"

using namespace std;

namespace downward::tasks {
CostAdaptedTask::CostAdaptedTask(
    const shared_ptr<AbstractTask>& parent,
    OperatorCost cost_type)
    : DelegatingTask(parent)
    , cost_type(cost_type)
    , parent_is_unit_cost(
          task_properties::is_unit_cost(parent->get_operators(), *parent))
{
}

int CostAdaptedTask::get_operator_cost(int index) const
{
    OperatorProxy op(*parent, index);
    return get_adjusted_action_cost(
        op,
        *parent,
        cost_type,
        parent_is_unit_cost);
}

} // namespace downward::tasks
