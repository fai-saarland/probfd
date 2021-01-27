#include "expected_cost_objective.h"

#include "../../global_state.h"
#include "../../globals.h"
#include "../../option_parser.h"
#include "../../plugin.h"

namespace probabilistic {

class GoalCheck : public GlobalStateEvaluator {
protected:
    EvaluationResult evaluate(const GlobalState& state) override
    {
        return { ::test_goal(state), value_type::zero };
    }
};

class UnitActionEvaluator :
    public ProbabilisticOperatorEvaluator {
public:
    ~UnitActionEvaluator() override = default;

    value_type::value_t
    evaluate(const StateID&, const ProbabilisticOperator* const&)
    const override
    {
        return -1;
    }
};

ExpectedCostObjective::ExpectedCostObjective()
    : state_eval_(new GoalCheck())
    , action_eval_(new UnitActionEvaluator())
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

} // namespace probabilistic
