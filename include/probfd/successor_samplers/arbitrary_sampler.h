#ifndef PROBFD_SUCCESSOR_SAMPLERS_ARBITRARY_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_ARBITRARY_SAMPLER_H

#include "probfd/algorithms/successor_sampler.h"

/// This namespace contains implementations of transition successor samplers.
namespace probfd::successor_samplers {

template <typename Action>
class ArbitrarySuccessorSampler : public algorithms::SuccessorSampler<Action> {
protected:
    StateID sample(
        StateID state,
        Action action,
        const SuccessorDistribution& successors,
        algorithms::StateProperties& properties) override;
};

} // namespace probfd::successor_samplers

#include "probfd/successor_samplers/arbitrary_sampler_impl.h"

#endif // PROBFD_SUCCESSOR_SAMPLERS_ARBITRARY_SAMPLER_H