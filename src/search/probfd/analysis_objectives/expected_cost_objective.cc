#include "probfd/analysis_objectives/expected_cost_objective.h"

#include "global_state.h"
#include "globals.h"
#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace analysis_objectives {

namespace {
class SSPReward : public GlobalRewardFunction {
protected:
    EvaluationResult evaluate(const GlobalState& state) override
    {
        return {::test_goal(state), value_type::zero};
    }

    value_type::value_t
    evaluate(StateID, const ProbabilisticOperator* op) override
    {
        return op->get_reward();
    }
};
} // namespace

ExpectedCostObjective::ExpectedCostObjective()
    : reward_(new SSPReward())
{
}

value_utils::IntervalValue ExpectedCostObjective::reward_bound()
{
    return value_utils::IntervalValue(-value_type::inf, value_type::zero);
}

GlobalRewardFunction* ExpectedCostObjective::reward()
{
    return reward_.get();
}

} // namespace analysis_objectives
} // namespace probfd
