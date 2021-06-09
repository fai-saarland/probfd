#pragma once

#include "../global_state.h"
#include "engine_interfaces/state_reward_function.h"

namespace probabilistic {

using GlobalStateRewardFunction = StateRewardFunction<GlobalState>;

} // namespace probabilistic
