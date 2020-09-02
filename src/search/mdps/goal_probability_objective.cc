#include "goal_probability_objective.h"

#include "../global_state.h"
#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"

namespace probabilistic {

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

class NoTransitionReward : public TransitionEvaluator {
protected:
#if 0
    virtual value_type::value_t
    evaluate(const GlobalState&, const ProbabilisticOperator* const&) override
    {
        return value_type::zero;
    }
    virtual value_type::value_t evaluate(
        const GlobalState&,
        const std::pair<GlobalState, const ProbabilisticOperator*>&) override
    {
        return value_type::zero;
    }
#endif
};

GoalProbabilityObjective::GoalProbabilityObjective() { }

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
GoalProbabilityObjective::state_evaluator()
{
    return new GoalCheck();
}

TransitionEvaluator*
GoalProbabilityObjective::transition_evaluator()
{
    return new NoTransitionReward();
}

} // namespace probabilistic
