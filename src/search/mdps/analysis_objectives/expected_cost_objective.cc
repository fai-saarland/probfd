#include "expected_cost_objective.h"

#include "../../global_state.h"
#include "../../globals.h"
#include "../../option_parser.h"
#include "../../plugin.h"

namespace probabilistic {
namespace analysis_objectives {

namespace {
class GoalCheck : public GlobalStateEvaluator {
protected:
    EvaluationResult evaluate(const GlobalState& state) override
    {
        return { ::test_goal(state), value_type::zero };
    }
};

class ActionCostEvaluator : public ProbabilisticOperatorEvaluator {
protected:
    value_type::value_t
    evaluate(StateID, const ProbabilisticOperator* op) override
    {
        return -op->get_cost();
    }
};
}

ExpectedCostObjective::ExpectedCostObjective()
    : state_eval_(new GoalCheck())
    , action_eval_(new ActionCostEvaluator())
{
}

value_type::value_t
ExpectedCostObjective::min()
{
    return -value_type::inf;
}

value_type::value_t
ExpectedCostObjective::max()
{
    return value_type::zero;
}

GlobalStateEvaluator*
ExpectedCostObjective::state_reward()
{
    return state_eval_.get();
}

ProbabilisticOperatorEvaluator*
ExpectedCostObjective::action_reward()
{
    return action_eval_.get();
}

} // namespace analysis_objectives
} // namespace probabilistic
