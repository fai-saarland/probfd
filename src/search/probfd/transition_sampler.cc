#include "probfd/transition_sampler.h"

#include "plugin.h"

namespace probfd {

StateID ProbabilisticOperatorTransitionSampler::operator()(
    const StateID& state,
    const ProbabilisticOperator* op,
    const Distribution<StateID>& transition)
{
    return this->sample(state, op, transition);
}

static PluginTypePlugin<ProbabilisticOperatorTransitionSampler>
    _plugin_type("TransitionSampler", "");

} // namespace probfd
