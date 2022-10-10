#ifndef MDPS_REWARD_FUNCTION_H
#define MDPS_REWARD_FUNCTION_H

#include "../global_state.h"
#include "probabilistic_operator.h"

#include "engine_interfaces/reward_function.h"

namespace probfd {

/// Type alias for state reward functions for GlobalState.
using GlobalRewardFunction =
    RewardFunction<GlobalState, const ProbabilisticOperator*>;

} // namespace probfd

#endif // __STATE_REWARD_FUNCTION_H__