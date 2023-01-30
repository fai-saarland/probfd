#include "probfd/analysis_objectives/expected_cost_objective.h"

#include "task_utils/task_properties.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace analysis_objectives {

namespace {
class SSPReward : public TaskRewardFunction {
    ProbabilisticTaskProxy task_proxy;

public:
    SSPReward(const ProbabilisticTaskProxy& task_proxy)
        : task_proxy(task_proxy)
    {
    }

protected:
    TerminationInfo evaluate(const State& state) override
    {
        if (task_properties::is_goal_state(task_proxy, state)) {
            return TerminationInfo(true, value_type::zero);
        } else {
            return TerminationInfo(false, -value_type::inf);
        }
    }

    value_type::value_t evaluate(StateID, OperatorID op) override
    {
        return task_proxy.get_operators()[op].get_reward();
    }
};
} // namespace

ExpectedCostObjective::ExpectedCostObjective()
    : reward_(new SSPReward(ProbabilisticTaskProxy(*tasks::g_root_task)))
{
}

value_utils::IntervalValue ExpectedCostObjective::reward_bound()
{
    return value_utils::IntervalValue(-value_type::inf, value_type::zero);
}

TaskRewardFunction* ExpectedCostObjective::reward()
{
    return reward_.get();
}

} // namespace analysis_objectives
} // namespace probfd
