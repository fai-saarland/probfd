#include "probfd/cost_models/maxprob_cost_model.h"

#include "probfd/tasks/root_task.h"

#include "probfd/cost_function.h"
#include "probfd/task_proxy.h"

#include "downward/task_utils/task_properties.h"

#include "downward/operator_id.h"

namespace probfd {
namespace cost_models {

namespace {
class MaxProbCostFunction : public TaskSimpleCostFunction {
    ProbabilisticTaskProxy task_proxy;

public:
    MaxProbCostFunction(const ProbabilisticTaskProxy& task_proxy)
        : task_proxy(task_proxy)
    {
    }

    bool is_goal(param_type<State> state) const override
    {
        return task_properties::is_goal_state(task_proxy, state);
    }

    value_t get_goal_termination_cost() const override final { return 0_vt; }
    value_t get_non_goal_termination_cost() const override final
    {
        return 1_vt;
    }

    value_t get_action_cost(OperatorID) override { return 0_vt; }
};
} // namespace

MaxProbCostModel::MaxProbCostModel()
    : cost_function_(
          new MaxProbCostFunction(ProbabilisticTaskProxy(*tasks::g_root_task)))
{
}

MaxProbCostModel::~MaxProbCostModel() = default;

Interval MaxProbCostModel::optimal_value_bound() const
{
    return Interval(0_vt, 1_vt);
}

TaskSimpleCostFunction* MaxProbCostModel::get_cost_function()
{
    return cost_function_.get();
}

} // namespace cost_models
} // namespace probfd
