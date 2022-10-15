#ifndef MDPS_TRANSITION_SAMPLER_H
#define MDPS_TRANSITION_SAMPLER_H

#include "probfd/engine_interfaces/transition_sampler.h"

#include "probfd/heuristic_search_interfaceable.h"
#include "probfd/probabilistic_operator.h"

#include <utility>

namespace probfd {
namespace engine_interfaces {

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

} // namespace engine_interfaces

using ProbabilisticOperatorTransitionSampler =
    engine_interfaces::TransitionSampler<const ProbabilisticOperator*>;

} // namespace probfd

#endif // __TRANSITION_SAMPLER_H__