#ifndef PROBFD_TASKS_RANGE_OPERATOR_COSTS_FUNCTION_H
#define PROBFD_TASKS_RANGE_OPERATOR_COSTS_FUNCTION_H

#include "probfd/value_type.h"

#include "downward/tasks/range_operator_cost_function.h"

namespace probfd::extra_tasks {

using VectorProbabilisticOperatorCostFunction =
    downward::extra_tasks::VectorOperatorCostFunction<value_t>;

} // namespace probfd::extra_tasks

#endif
