#include "probfd/analysis_objectives/expected_cost_objective.h"

#include "legacy/global_state.h"
#include "legacy/globals.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace analysis_objectives {

namespace {
class SSPReward : public GlobalRewardFunction {
protected:
    TerminationInfo evaluate(const legacy::GlobalState& state) override
    {
        if (legacy::test_goal(state)) {
            return TerminationInfo(true, value_type::zero);
        } else {
            return TerminationInfo(false, -value_type::inf);
        }
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
