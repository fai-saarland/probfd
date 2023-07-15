#include "probfd/cost_models/maxprob_cost_model.h"

#include "probfd/engine_interfaces/cost_function.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "downward/task_utils/task_properties.h"

#include "downward/operator_id.h"

namespace probfd {
namespace cost_models {

namespace {
class MaxProbCostFunction : public TaskCostFunction {
    ProbabilisticTaskProxy task_proxy;

public:
    MaxProbCostFunction(const ProbabilisticTaskProxy& task_proxy)
        : TaskCostFunction(0_vt, 1_vt)
        , task_proxy(task_proxy)
    {
    }

    bool is_goal(param_type<State> state) const override
    {
        return task_properties::is_goal_state(task_proxy, state);
    }

    value_t get_action_cost(OperatorID) override { return 0_vt; }
};
} // namespace

MaxProbCostModel::MaxProbCostModel()
    : cost_function_(
          new MaxProbCostFunction(ProbabilisticTaskProxy(*tasks::g_root_task)))
{
}

Interval MaxProbCostModel::optimal_value_bound() const
{
    return Interval(0_vt, 1_vt);
}

TaskCostFunction* MaxProbCostModel::get_cost_function()
{
    return cost_function_.get();
}

} // namespace cost_models
} // namespace probfd
