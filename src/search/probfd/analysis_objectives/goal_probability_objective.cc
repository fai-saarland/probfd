#include "probfd/analysis_objectives/goal_probability_objective.h"

#include "task_utils/task_properties.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace analysis_objectives {

namespace {
class MaxProbReward : public TaskRewardFunction {
    ProbabilisticTaskProxy task_proxy;

public:
    MaxProbReward(const ProbabilisticTaskProxy& task_proxy)
        : task_proxy(task_proxy)
    {
    }

protected:
    virtual TerminationInfo evaluate(const State& state) override
    {
        ProbabilisticTaskProxy task_proxy(*tasks::g_root_task);
        if (task_properties::is_goal_state(task_proxy, state)) {
            return TerminationInfo(true, value_type::one);
        } else {
            return TerminationInfo(false, value_type::zero);
        }
    }

    value_type::value_t evaluate(StateID, OperatorID) override
    {
        return value_type::zero;
    }
};
} // namespace

GoalProbabilityObjective::GoalProbabilityObjective()
    : reward_(new MaxProbReward(ProbabilisticTaskProxy(*tasks::g_root_task)))
{
}

value_utils::IntervalValue GoalProbabilityObjective::reward_bound()
{
    return value_utils::IntervalValue(value_type::zero, value_type::one);
}

TaskRewardFunction* GoalProbabilityObjective::reward()
{
    return reward_.get();
}

} // namespace analysis_objectives
} // namespace probfd
