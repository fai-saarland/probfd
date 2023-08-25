#ifndef PROBFD_SUCCESSOR_SAMPLERS_MOST_LIKELY_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_MOST_LIKELY_SAMPLER_H

#include "probfd/algorithms/successor_sampler.h"

namespace probfd {
namespace successor_samplers {

template <typename Action>
class MostLikelySuccessorSampler : public algorithms::SuccessorSampler<Action> {
protected:
    StateID sample(
        StateID state,
        Action action,
        const Distribution<StateID>& successors,
        algorithms::StateProperties& properties) override;
};

} // namespace successor_samplers
} // namespace probfd

#include "probfd/successor_samplers/most_likely_sampler_impl.h"

#endif // __MOST_LIKELY_SELECTOR_H__