#pragma once

#include "../probabilistic_operator.h"
#include "../transition_sampler.h"
#include "../utils/distribution_uniform_sampler.h"

namespace probabilistic {
namespace transition_sampler {

class UniformSuccessorSampler : public ProbabilisticOperatorTransitionSampler {
protected:
    virtual StateID sample(
        const StateID& state,
        const ProbabilisticOperator* op,
        const Distribution<StateID>& successors) override;

    distribution_uniform_sampler::DistributionUniformSampler sampler_;
};

} // namespace transition_sampler
} // namespace probabilistic
