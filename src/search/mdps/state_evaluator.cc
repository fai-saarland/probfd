#include "state_evaluator.h"

#include "../plugin.h"

namespace probabilistic {
namespace algorithms {

EvaluationResult
StateEvaluationFunction<GlobalState>::operator()(const GlobalState& state)
{
    return this->evaluate(state);
}

static PluginTypePlugin<GlobalStateEvaluator>
    _plugin("GlobalStateEvaluator", "");

} // namespace algorithms
} // namespace probabilistic
