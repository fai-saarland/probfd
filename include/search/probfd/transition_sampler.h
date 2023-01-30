#ifndef MDPS_TRANSITION_SAMPLER_H
#define MDPS_TRANSITION_SAMPLER_H

#include "probfd/engine_interfaces/transition_sampler.h"

#include "probfd/heuristic_search_interfaceable.h"

#include <utility>

namespace probfd {
namespace engine_interfaces {

template <>
class TransitionSampler<OperatorID> : public HeuristicSearchInterfaceable {
public:
    StateID operator()(
        const StateID& state,
        OperatorID op,
        const Distribution<StateID>& transition);

protected:
    virtual StateID sample(
        const StateID& state,
        OperatorID op,
        const Distribution<StateID>& transition) = 0;
};

} // namespace engine_interfaces

using ProbabilisticOperatorTransitionSampler =
    engine_interfaces::TransitionSampler<OperatorID>;

} // namespace probfd

#endif // __TRANSITION_SAMPLER_H__