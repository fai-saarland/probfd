#include "probfd/tasks/cost_adapted_task.h"

#include "probfd/task_utils/task_properties.h"

#include "downward/operator_cost.h"

#include "downward/plugins/plugin.h"

#include "probfd/tasks/root_task.h"

#include "downward/utils/system.h"

#include <iostream>
#include <memory>

using namespace std;
using utils::ExitCode;

namespace probfd {
namespace tasks {

CostAdaptedTask::CostAdaptedTask(
    const shared_ptr<ProbabilisticTask>& parent,
    OperatorCost cost_type)
    : DelegatingTask(parent)
    , cost_type(cost_type)
    , parent_is_unit_cost(
          task_properties::is_unit_cost(ProbabilisticTaskProxy(*parent)))
{
}

value_t CostAdaptedTask::get_operator_cost(int index) const
{
    ProbabilisticOperatorProxy op(*parent, index);
    return task_properties::get_adjusted_action_cost(
        op,
        cost_type,
        parent_is_unit_cost);
}

} // namespace tasks
} // namespace probfd