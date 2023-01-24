#include "probfd/reward_models/ssp_reward_model.h"

#include "task_utils/task_properties.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace reward_models {

namespace {
class SSPRewardFunction : public TaskRewardFunction {
    ProbabilisticTaskProxy task_proxy;

public:
    SSPRewardFunction(const ProbabilisticTaskProxy& task_proxy)
        : task_proxy(task_proxy)
    {
    }

    TerminationInfo get_termination_info(const State& state) override
    {
        if (task_properties::is_goal_state(task_proxy, state)) {
            return TerminationInfo(true, 0_vt);
        } else {
            return TerminationInfo(false, -INFINITE_VALUE);
        }
    }

    value_t get_action_reward(StateID, OperatorID op) override
    {
        return task_proxy.get_operators()[op].get_reward();
    }
};
} // namespace

SSPRewardModel::SSPRewardModel()
    : reward_(
          new SSPRewardFunction(ProbabilisticTaskProxy(*tasks::g_root_task)))
{
}

Interval SSPRewardModel::reward_bound()
{
    return Interval(-INFINITE_VALUE, 0_vt);
}

TaskRewardFunction* SSPRewardModel::get_reward_function()
{
    return reward_.get();
}

} // namespace reward_models
} // namespace probfd
