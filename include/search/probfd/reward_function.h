#ifndef MDPS_REWARD_FUNCTION_H
#define MDPS_REWARD_FUNCTION_H

#include "probfd/engine_interfaces/reward_function.h"

#include "task_proxy.h"

namespace probfd {

/// Type alias for state reward functions for GlobalState.
using GlobalRewardFunction =
    engine_interfaces::RewardFunction<State, OperatorID>;

} // namespace probfd

#endif // __STATE_REWARD_FUNCTION_H__