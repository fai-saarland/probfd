#ifndef MDPS_TRANSITION_SAMPLER_RANDOM_SUCCESSOR_SAMPLER_H
#define MDPS_TRANSITION_SAMPLER_RANDOM_SUCCESSOR_SAMPLER_H

#include "../transition_sampler.h"
#include "../utils/distribution_random_sampler.h"

namespace probfd {
namespace transition_sampler {

class RandomSuccessorSampler : public ProbabilisticOperatorTransitionSampler {
protected:
    virtual StateID sample(
        const StateID& state,
        const ProbabilisticOperator* op,
        const Distribution<StateID>& successors) override;

    distribution_random_sampler::DistributionRandomSampler sampler_;
};

} // namespace transition_sampler
} // namespace probfd

#endif // __RANDOM_SUCCESSOR_SAMPLER_H__