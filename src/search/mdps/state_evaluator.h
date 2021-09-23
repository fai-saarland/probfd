#ifndef MDPS_STATE_EVALUATOR_H
#define MDPS_STATE_EVALUATOR_H

#include "../global_state.h"
#include "engine_interfaces/state_evaluator.h"

namespace probabilistic {

/// Type alias for state evaluators of \ref GlobalState.
using GlobalStateEvaluator = StateEvaluator<GlobalState>;

} // namespace probabilistic

#endif // __STATE_EVALUATOR_H__