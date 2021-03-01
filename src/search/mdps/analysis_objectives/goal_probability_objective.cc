#include "goal_probability_objective.h"

#include "../../global_state.h"
#include "../../globals.h"
#include "../../option_parser.h"
#include "../../plugin.h"

namespace probabilistic {

namespace {
class GoalCheck : public GlobalStateEvaluator {
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

class ZeroRewardActionEvaluator : public ProbabilisticOperatorEvaluator {
protected:
    value_type::value_t evaluate(
    const StateID&, const ProbabilisticOperator* const&) const override {
        return value_type::zero;
    }
};
}

GoalProbabilityObjective::GoalProbabilityObjective()
    : state_eval_(new GoalCheck())
    , action_eval_(new ZeroRewardActionEvaluator())
{
}

value_type::value_t
GoalProbabilityObjective::min()
{
    return value_type::zero;
}

value_type::value_t
GoalProbabilityObjective::max()
{
    return value_type::one;
}

GlobalStateEvaluator*
GoalProbabilityObjective::state_reward()
{
    return state_eval_.get();
}

ProbabilisticOperatorEvaluator*
GoalProbabilityObjective::action_reward()
{
    return action_eval_.get();
}

} // namespace probabilistic
