#include "probfd/cost_models/ssp_cost_model.h"

#include "downward/task_utils/task_properties.h"

#include "probfd/tasks/root_task.h"

#include "probfd/cost_function.h"
#include "probfd/task_proxy.h"

namespace probfd {
namespace cost_models {

namespace {
class SSPCostFunction : public TaskSimpleCostFunction {
    ProbabilisticTaskProxy task_proxy;

public:
    SSPCostFunction(const ProbabilisticTaskProxy& task_proxy)
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
        return INFINITE_VALUE;
    }

    value_t get_action_cost(OperatorID op) override
    {
        return task_proxy.get_operators()[op].get_cost();
    }
};
} // namespace

SSPCostModel::SSPCostModel()
    : cost_function_(
          new SSPCostFunction(ProbabilisticTaskProxy(*tasks::g_root_task)))
{
}

SSPCostModel::~SSPCostModel() = default;

Interval SSPCostModel::optimal_value_bound() const
{
    return Interval(0_vt, INFINITE_VALUE);
}

TaskSimpleCostFunction* SSPCostModel::get_cost_function()
{
    return cost_function_.get();
}

} // namespace cost_models
} // namespace probfd
