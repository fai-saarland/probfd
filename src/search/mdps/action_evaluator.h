#pragma once

#include "engine_interfaces/action_evaluator.h"
#include "probabilistic_operator.h"

namespace probabilistic {

using ProbabilisticOperatorEvaluator =
    ActionRewardFunction<const ProbabilisticOperator*>;

} // namespace probabilistic
