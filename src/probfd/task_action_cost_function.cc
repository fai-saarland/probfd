#include "probfd/task_action_cost_function.h"

#include "downward/operator_cost_function.h"

using namespace downward;

namespace probfd {

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