#ifndef PROBFD_TASK_COST_FUNCTION_H
#define PROBFD_TASK_COST_FUNCTION_H

#include "probfd/cost_function.h"

#include "downward/task_utils/task_properties.h"

namespace downward {
class GoalFactList;
}

namespace probfd {
class TerminationCosts;
} // namespace probfd

namespace probfd {

class TaskTerminationCostFunction
    : public SimpleTerminationCostFunction<downward::State> {
    std::shared_ptr<downward::GoalFactList> goals_;
    std::shared_ptr<TerminationCosts> costs_;

public:
    explicit TaskTerminationCostFunction(
        std::shared_ptr<downward::GoalFactList> goals,
        std::shared_ptr<TerminationCosts> costs);

    bool is_goal(const downward::State& state) const override;

    value_t get_goal_termination_cost() const override;
    value_t get_non_goal_termination_cost() const override;
};

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
