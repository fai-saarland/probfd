#ifndef MDPS_ACTION_EVALUATOR_H
#define MDPS_ACTION_EVALUATOR_H

#include "engine_interfaces/action_evaluator.h"
#include "probabilistic_operator.h"

namespace probabilistic {

/// Type alias for action evaluators of \ref ProbabilisticOperator.
using ProbabilisticOperatorEvaluator =
    ActionRewardFunction<const ProbabilisticOperator*>;

} // namespace probabilistic

#endif // __ACTION_EVALUATOR_H__