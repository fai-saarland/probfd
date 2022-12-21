#ifndef MDPS_REWARD_FUNCTION_H
#define MDPS_REWARD_FUNCTION_H

#include "probfd/engine_interfaces/reward_function.h"

#include "probfd/probabilistic_operator.h"

#include "legacy/global_state.h"

namespace probfd {

/// Type alias for state reward functions for GlobalState.
using GlobalRewardFunction = engine_interfaces::
    RewardFunction<legacy::GlobalState, const ProbabilisticOperator*>;

} // namespace probfd

#endif // __STATE_REWARD_FUNCTION_H__