#include "transition_sampler.h"

#include "../plugin.h"

namespace probabilistic {
namespace algorithms {
GlobalState
TransitionSampler<GlobalState, const ProbabilisticOperator*>::operator()(
    const GlobalState& state,
    const ProbabilisticOperator* op,
    const Distribution<GlobalState>& transition)
{
    return this->sample(state, op, transition);
}

void
TransitionSampler<GlobalState, const ProbabilisticOperator*>::connect(
    algorithms::HeuristicSearchStatusInterface<
        GlobalState,
        const ProbabilisticOperator*>* e)
{
    hs_interface_ = e;
}

} // namespace algorithms
static PluginTypePlugin<TransitionSampler>
    _plugin_type("TransitionSampler", "");
} // namespace probabilistic
