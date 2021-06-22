#pragma once

#include "analysis_objective.h"

#include <memory>

namespace probabilistic {

class ProbabilisticOperator;

namespace analysis_objectives {

class GoalProbabilityObjective : public AnalysisObjective {
public:
    explicit GoalProbabilityObjective();

    virtual value_type::value_t min() override;
    virtual value_type::value_t max() override;
    virtual GlobalStateRewardFunction* state_reward() override;
    virtual ProbabilisticOperatorEvaluator* action_reward() override;

private:
    std::unique_ptr<GlobalStateRewardFunction> state_eval_;
    std::unique_ptr<ProbabilisticOperatorEvaluator> action_eval_;
};

} // namespace analysis_objectives
} // namespace probabilistic
