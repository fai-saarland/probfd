#include "probfd/cost_models/ssp_cost_model.h"

#include "task_utils/task_properties.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace cost_models {

namespace {
class SSPCostFunction : public TaskCostFunction {
    ProbabilisticTaskProxy task_proxy;

public:
    SSPCostFunction(const ProbabilisticTaskProxy& task_proxy)
        : TaskCostFunction(0_vt, INFINITE_VALUE)
        , task_proxy(task_proxy)
    {
    }

    bool is_goal(param_type<State> state) const override
    {
        return task_properties::is_goal_state(task_proxy, state);
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

Interval SSPCostModel::optimal_value_bound() const
{
    return Interval(0_vt, INFINITE_VALUE);
}

TaskCostFunction* SSPCostModel::get_cost_function()
{
    return cost_function_.get();
}

} // namespace cost_models
} // namespace probfd
