#include "state_evaluator.h"

#include "../plugin.h"

namespace probabilistic {

EvaluationResult
StateEvaluator<GlobalState>::operator()(const GlobalState& state)
{
    return this->evaluate(state);
}

static PluginTypePlugin<GlobalStateEvaluator>
    _plugin("GlobalStateEvaluator", "");

} // namespace probabilistic
