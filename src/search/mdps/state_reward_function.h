#pragma once

#include "../global_state.h"
#include "engine_interfaces/state_reward_function.h"

namespace probabilistic {

/// Type alias for state reward functions of \ref GlobalState.
using GlobalStateRewardFunction = StateRewardFunction<GlobalState>;

} // namespace probabilistic
