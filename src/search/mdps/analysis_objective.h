#pragma once

#include "state_evaluator.h"
#include "transition_evaluator.h"
#include "value_type.h"

#include <memory>

namespace probabilistic {

class AnalysisObjective {
public:
    virtual ~AnalysisObjective() = default;
    virtual value_type::value_t min() = 0;
    virtual value_type::value_t max() = 0;
    virtual GlobalStateEvaluator* state_evaluator() = 0;
    virtual TransitionEvaluator* transition_evaluator() = 0;
};

} // namespace probabilistic
