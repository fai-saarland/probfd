#ifndef MDPS_TRANSITION_SAMPLER_H
#define MDPS_TRANSITION_SAMPLER_H

#include "engine_interfaces/transition_sampler.h"
#include "heuristic_search_interfaceable.h"
#include "probabilistic_operator.h"

#include <utility>

namespace probfd {
template <>
class TransitionSampler<const ProbabilisticOperator*>
    : public HeuristicSearchInterfaceable {
public:
    StateID operator()(
        const StateID& state,
        const ProbabilisticOperator* op,
        const Distribution<StateID>& transition);

protected:
    virtual StateID sample(
        const StateID& state,
        const ProbabilisticOperator* op,
        const Distribution<StateID>& transition) = 0;
};

using ProbabilisticOperatorTransitionSampler =
    TransitionSampler<const ProbabilisticOperator*>;

} // namespace probfd

#endif // __TRANSITION_SAMPLER_H__