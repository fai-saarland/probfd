#pragma once

#include "../../global_state.h"
#include "../probabilistic_operator.h"
#include "../transition_sampler.h"
#include "../utils/distribution_uniform_sampler.h"

namespace probabilistic {
namespace successor_sampler {

class UniformSuccessorSampler : public TransitionSampler {
protected:
    virtual GlobalState sample(
        const GlobalState& state,
        const ProbabilisticOperator* op,
        const Distribution<GlobalState>& successors) override;

    distribution_uniform_sampler::DistributionUniformSampler sampler_;
};

} // namespace successor_sampler
} // namespace probabilistic

