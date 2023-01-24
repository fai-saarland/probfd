#include "probfd/reward_models/maxprob_reward_model.h"

#include "task_utils/task_properties.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace reward_models {

namespace {
class MaxProbRewardFunction : public TaskRewardFunction {
    ProbabilisticTaskProxy task_proxy;

public:
    MaxProbRewardFunction(const ProbabilisticTaskProxy& task_proxy)
        : task_proxy(task_proxy)
    {
    }

    virtual TerminationInfo get_termination_info(const State& state) override
    {
        ProbabilisticTaskProxy task_proxy(*tasks::g_root_task);
        if (task_properties::is_goal_state(task_proxy, state)) {
            return TerminationInfo(true, 1_vt);
        } else {
            return TerminationInfo(false, 0_vt);
        }
    }

    value_t get_action_reward(StateID, OperatorID) override
    {
        return 0_vt;
    }
};
} // namespace

MaxProbRewardModel::MaxProbRewardModel()
    : reward_(new MaxProbRewardFunction(
          ProbabilisticTaskProxy(*tasks::g_root_task)))
{
}

Interval MaxProbRewardModel::reward_bound()
{
    return Interval(0_vt, 1_vt);
}

TaskRewardFunction* MaxProbRewardModel::get_reward_function()
{
    return reward_.get();
}

} // namespace reward_models
} // namespace probfd
