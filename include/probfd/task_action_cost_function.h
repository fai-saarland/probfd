#ifndef PROBFD_TASK_ACTION_COST_FUNCTION_H
#define PROBFD_TASK_ACTION_COST_FUNCTION_H

#include "probfd/action_cost_function.h"

#include "downward/operator_id.h"
#include "downward/operator_cost_function_fwd.h"

#include <memory>

namespace probfd {

class TaskActionCostFunction : public ActionCostFunction<downward::OperatorID> {
    std::shared_ptr<downward::OperatorCostFunction<value_t>> op_cost_function_;

public:
    explicit TaskActionCostFunction(
        std::shared_ptr<downward::OperatorCostFunction<value_t>>
            op_cost_function);

    value_t get_action_cost(downward::OperatorID action) override;
};

} // namespace probfd

#endif
