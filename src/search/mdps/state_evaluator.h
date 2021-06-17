#pragma once

#include "../global_state.h"
#include "engine_interfaces/state_evaluator.h"

namespace probabilistic {

/// Type alias for state evaluators of \ref GlobalState.
using GlobalStateEvaluator = StateEvaluator<GlobalState>;

} // namespace probabilistic
