#ifndef PROBFD_SUCCESSOR_SAMPLERS_MOST_LIKELY_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_MOST_LIKELY_SAMPLER_H

#include "probfd/algorithms/successor_sampler.h"

namespace probfd::successor_samplers {

template <typename Action>
class MostLikelySuccessorSampler : public algorithms::SuccessorSampler<Action> {
protected:
    StateID sample(
        StateID state,
        Action action,
        const SuccessorDistribution& successors,
        algorithms::StateProperties& properties) override;
};

} // namespace probfd::successor_samplers

#include "probfd/successor_samplers/most_likely_sampler_impl.h"

#endif // PROBFD_SUCCESSOR_SAMPLERS_MOST_LIKELY_SAMPLER_H