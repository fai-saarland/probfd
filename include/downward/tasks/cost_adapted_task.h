#ifndef TASKS_COST_ADAPTED_TASK_H
#define TASKS_COST_ADAPTED_TASK_H

#include "downward/operator_cost_function.h"

#include "downward/operator_cost.h"

#include "downward/task_utils/task_properties.h"

#include <memory>

namespace downward {
class OperatorSpace;
}

namespace downward::tasks {
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
template <typename T>
class AdaptedOperatorCostFunction : public OperatorCostFunction<T> {
    std::shared_ptr<OperatorCostFunction<T>> parent;
    const OperatorCost cost_type;
    const bool parent_is_unit_cost;

public:
    AdaptedOperatorCostFunction(
        const OperatorSpace& operators,
        std::shared_ptr<OperatorCostFunction<T>> parent,
        OperatorCost cost_type)
        : parent(std::move(parent))
        , cost_type(cost_type)
        , parent_is_unit_cost(
              task_properties::is_unit_cost(operators, *this->parent))
    {
    }

    int get_operator_cost(int index) const override
    {
        return get_adjusted_action_cost(
            index,
            *parent,
            cost_type,
            parent_is_unit_cost);
    }
};

using AdaptedOperatorIntCostFunction = AdaptedOperatorCostFunction<int>;

} // namespace downward::tasks

#endif
