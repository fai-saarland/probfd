#include "probfd/tasks/cost_adapted_task.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/task_proxy.h"

#include "downward/operator_cost.h"

#include <memory>

using namespace std;

using namespace downward;

namespace probfd::tasks {

CostAdaptedTask::CostAdaptedTask(
    const shared_ptr<ProbabilisticTask>& parent,
    OperatorCost cost_type)
    : DelegatingTask(parent)
    , cost_type_(cost_type)
    , parent_is_unit_cost_(
          task_properties::is_unit_cost(ProbabilisticTaskProxy(*parent)))
{
}

value_t CostAdaptedTask::get_operator_cost(int index) const
{
    ProbabilisticOperatorProxy op(*parent_, index);
    return task_properties::get_adjusted_action_cost(
        op,
        cost_type_,
        parent_is_unit_cost_);
}

} // namespace probfd::tasks
