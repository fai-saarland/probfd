#include "goal_probability_objective.h"

#include "../../global_state.h"
#include "../../globals.h"
#include "../../option_parser.h"
#include "../../plugin.h"

namespace probfd {
namespace analysis_objectives {

namespace {
class MaxProbReward : public GlobalRewardFunction {
protected:
    virtual EvaluationResult evaluate(const GlobalState& state) override
    {
        if (::test_goal(state)) {
            return EvaluationResult(true, value_type::one);
        } else {
            return EvaluationResult(false, value_type::zero);
        }
    }

    value_type::value_t evaluate(StateID, const ProbabilisticOperator*) override
    {
        return value_type::zero;
    }
};
} // namespace

GoalProbabilityObjective::GoalProbabilityObjective()
    : reward_(new MaxProbReward())
{
}

value_utils::IntervalValue GoalProbabilityObjective::reward_bound()
{
    return value_utils::IntervalValue(value_type::zero, value_type::one);
}

GlobalRewardFunction* GoalProbabilityObjective::reward()
{
    return reward_.get();
}

} // namespace analysis_objectives
} // namespace probfd
