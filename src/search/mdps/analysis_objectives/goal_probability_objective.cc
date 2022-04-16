#include "goal_probability_objective.h"

#include "../../global_state.h"
#include "../../globals.h"
#include "../../option_parser.h"
#include "../../plugin.h"

namespace probabilistic {
namespace analysis_objectives {

namespace {
class GoalCheck : public GlobalStateRewardFunction {
protected:
    virtual EvaluationResult evaluate(const GlobalState& state) override
    {
        if (::test_goal(state)) {
            return EvaluationResult(true, value_type::one);
        } else {
            return EvaluationResult(false, value_type::zero);
        }
    }
};

class ZeroRewardActionEvaluator : public ProbabilisticOperatorRewardFunction {
protected:
    value_type::value_t evaluate(StateID, const ProbabilisticOperator*) override
    {
        return value_type::zero;
    }
};
} // namespace

GoalProbabilityObjective::GoalProbabilityObjective()
    : state_eval_(new GoalCheck())
    , action_eval_(new ZeroRewardActionEvaluator())
{
}

value_utils::IntervalValue GoalProbabilityObjective::reward_bound()
{
    return value_utils::IntervalValue(value_type::zero, value_type::one);
}

GlobalStateRewardFunction* GoalProbabilityObjective::state_reward()
{
    return state_eval_.get();
}

ProbabilisticOperatorRewardFunction* GoalProbabilityObjective::action_reward()
{
    return action_eval_.get();
}

} // namespace analysis_objectives
} // namespace probabilistic
