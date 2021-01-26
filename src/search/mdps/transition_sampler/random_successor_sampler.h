#pragma once

#include "../transition_sampler.h"
#include "../utils/distribution_random_sampler.h"

namespace probabilistic {
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
} // namespace probabilistic

