#ifndef PROBFD_TASKS_COST_ADAPTED_TASK_H
#define PROBFD_TASKS_COST_ADAPTED_TASK_H

#include "probfd/tasks/delegating_task.h" // IWYU pragma: export

#include "probfd/value_type.h"

#include "downward/operator_cost.h"

#include <memory>

namespace probfd::tasks {

/*
  Task transformation that changes operator costs. If the parent task assigns
  costs 'c' to an operator, its adjusted costs, depending on the value of the
  cost_type option, are:

    NORMAL:  c
    ONE:     1
    PLUSONE: 1, if all operators have cost 1 in the parent task, else c + 1

  Regardless of the cost_type value, axioms will always keep their original
  cost, which is 0 by default.
*/
class CostAdaptedTask : public DelegatingTask {
    const OperatorCost cost_type_;
    const bool parent_is_unit_cost_;

public:
    CostAdaptedTask(
        const std::shared_ptr<ProbabilisticTask>& parent,
        OperatorCost cost_type);

    value_t get_operator_cost(int index) const override;
};

} // namespace probfd::tasks

#endif
