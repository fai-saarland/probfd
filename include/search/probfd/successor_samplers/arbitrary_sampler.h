#ifndef PROBFD_SUCCESSOR_SAMPLERS_ARBITRARY_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_ARBITRARY_SAMPLER_H

#include "probfd/algorithms/successor_sampler.h"

namespace probfd {

/// This namespace contains implementations of transition successor samplers.
namespace successor_samplers {

template <typename Action>
class ArbitrarySuccessorSampler : public algorithms::SuccessorSampler<Action> {
protected:
    StateID sample(
        StateID state,
        Action action,
        const Distribution<StateID>& successors,
        algorithms::StateProperties& properties) override;
};

} // namespace successor_samplers
} // namespace probfd

#include "probfd/successor_samplers/arbitrary_sampler_impl.h"

#endif // __ARBITRARY_SELECTOR_H__