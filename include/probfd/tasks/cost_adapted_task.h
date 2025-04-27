#ifndef PROBFD_TASKS_COST_ADAPTED_TASK_H
#define PROBFD_TASKS_COST_ADAPTED_TASK_H

#include "probfd/aliases.h"

#include "downward/tasks/cost_adapted_task.h"

namespace probfd::tasks {

using CostAdaptedProbabilisticCostFunction =
    downward::tasks::AdaptedOperatorCostFunction<value_t>;

} // namespace probfd::tasks

#endif
