#ifndef PROBFD_TASKS_MODIFIED_OPERATOR_COSTS_TASK_H
#define PROBFD_TASKS_MODIFIED_OPERATOR_COSTS_TASK_H

#include "probfd/value_type.h"

#include "downward/tasks/modified_operator_costs_task.h"

namespace probfd::extra_tasks {

using VectorProbabilisticOperatorCostFunction =
    downward::extra_tasks::VectorOperatorCostFunction<value_t>;

} // namespace probfd::extra_tasks

#endif
