#ifndef MDPS_STATE_REWARD_FUNCTION_H
#define MDPS_STATE_REWARD_FUNCTION_H

#include "../global_state.h"
#include "engine_interfaces/state_reward_function.h"

namespace probabilistic {

/// Type alias for state reward functions of \ref GlobalState.
using GlobalStateRewardFunction = StateRewardFunction<GlobalState>;

} // namespace probabilistic

#endif // __STATE_REWARD_FUNCTION_H__