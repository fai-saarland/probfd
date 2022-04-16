#include "expected_cost_objective.h"

#include "../../global_state.h"
#include "../../globals.h"
#include "../../option_parser.h"
#include "../../plugin.h"

namespace probabilistic {
namespace analysis_objectives {

namespace {
class GoalCheck : public GlobalStateRewardFunction {
protected:
    EvaluationResult evaluate(const GlobalState& state) override
    {
        return {::test_goal(state), value_type::zero};
    }
};

class ActionCostEvaluator : public ProbabilisticOperatorRewardFunction {
protected:
    value_type::value_t
    evaluate(StateID, const ProbabilisticOperator* op) override
    {
        return -op->get_cost();
    }
};
} // namespace

ExpectedCostObjective::ExpectedCostObjective()
    : state_eval_(new GoalCheck())
    , action_eval_(new ActionCostEvaluator())
{
}

value_utils::IntervalValue ExpectedCostObjective::reward_bound()
{
    return value_utils::IntervalValue(-value_type::inf, value_type::zero);
}

GlobalStateRewardFunction* ExpectedCostObjective::state_reward()
{
    return state_eval_.get();
}

ProbabilisticOperatorRewardFunction* ExpectedCostObjective::action_reward()
{
    return action_eval_.get();
}

} // namespace analysis_objectives
} // namespace probabilistic
