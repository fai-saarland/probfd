#pragma once

#include "../analysis_objective.h"

#include <memory>

namespace probabilistic {

class ProbabilisticOperator;

class ExpectedCostObjective : public AnalysisObjective {
public:
    explicit ExpectedCostObjective();

    virtual value_type::value_t min() override;
    virtual value_type::value_t max() override;
    virtual GlobalStateEvaluator* state_reward() override;
    virtual ProbabilisticOperatorEvaluator* action_reward() override;

private:
    std::unique_ptr<GlobalStateEvaluator> state_eval_;
    std::unique_ptr<ProbabilisticOperatorEvaluator> action_eval_;
};

} // namespace probabilistic
