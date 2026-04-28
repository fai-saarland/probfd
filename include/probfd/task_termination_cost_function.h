#ifndef PROBFD_TASK_TERMINATION_COST_FUNCTION_H
#define PROBFD_TASK_TERMINATION_COST_FUNCTION_H

#include "probfd/termination_cost_function.h"

#include "downward/state.h"

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

} // namespace probfd

#endif
