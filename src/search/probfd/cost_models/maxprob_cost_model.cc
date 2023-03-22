#include "probfd/cost_models/maxprob_cost_model.h"

#include "task_utils/task_properties.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "operator_id.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace cost_models {

namespace {
class MaxProbCostFunction : public TaskCostFunction {
    ProbabilisticTaskProxy task_proxy;

public:
    MaxProbCostFunction(const ProbabilisticTaskProxy& task_proxy)
        : task_proxy(task_proxy)
    {
    }

    virtual TerminationInfo get_termination_info(const State& state) override
    {
        ProbabilisticTaskProxy task_proxy(*tasks::g_root_task);
        if (task_properties::is_goal_state(task_proxy, state)) {
            return TerminationInfo(true, 0_vt);
        } else {
            return TerminationInfo(false, 1_vt);
        }
    }

    value_t get_action_cost(StateID, OperatorID) override { return 0_vt; }
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
