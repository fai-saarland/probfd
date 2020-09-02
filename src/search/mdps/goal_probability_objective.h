#pragma once

#include "analysis_objective.h"

class GlobalState;

namespace probabilistic {

class ProbabilisticOperator;

class GoalProbabilityObjective : public AnalysisObjective {
public:
    explicit GoalProbabilityObjective();

    virtual value_type::value_t min() override;
    virtual value_type::value_t max() override;
    virtual GlobalStateEvaluator* state_evaluator() override;
    virtual TransitionEvaluator* transition_evaluator() override;
};

} // namespace probabilistic
