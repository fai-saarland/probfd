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
        : task_proxy(task_proxy)
    {
    }

    TerminationInfo get_termination_info(const State& state) override
    {
        if (task_properties::is_goal_state(task_proxy, state)) {
            return TerminationInfo(true, 0_vt);
        } else {
            return TerminationInfo(false, INFINITE_VALUE);
        }
    }

    value_t get_action_cost(StateID, OperatorID op) override
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
