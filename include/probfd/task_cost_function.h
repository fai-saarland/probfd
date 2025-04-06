#ifndef PROBFD_TASK_COST_FUNCTION_H
#define PROBFD_TASK_COST_FUNCTION_H

#include "probfd/cost_function.h"

#include "probfd/task_proxy.h"

#include "downward/task_utils/task_properties.h"

namespace probfd {

class TaskCostFunction
    : public SimpleCostFunction<downward::State, downward::OperatorID> {
    std::shared_ptr<ProbabilisticTask> task_;

public:
    explicit TaskCostFunction(std::shared_ptr<ProbabilisticTask> task);

    bool is_goal(const downward::State& state) const override;

    value_t get_goal_termination_cost() const override;

    value_t get_non_goal_termination_cost() const override;

    value_t get_action_cost(downward::OperatorID action) override;
};

} // namespace probfd

#endif
