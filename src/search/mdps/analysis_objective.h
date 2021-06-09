#pragma once

#include "action_evaluator.h"
#include "state_reward_function.h"
#include "value_type.h"

#include <memory>

namespace probabilistic {
namespace analysis_objectives {

class AnalysisObjective {
public:
    virtual ~AnalysisObjective() = default;
    virtual value_type::value_t min() = 0;
    virtual value_type::value_t max() = 0;
    virtual GlobalStateRewardFunction* state_reward() = 0;
    virtual ProbabilisticOperatorEvaluator* action_reward() = 0;
};

} // analysis_objectives
} // namespace probabilistic
