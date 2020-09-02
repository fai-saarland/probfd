#pragma once

#include "../../global_state.h"
#include "../transition_sampler.h"
#include "../utils/distribution_random_sampler.h"

namespace probabilistic {
namespace successor_sampler {

class RandomSuccessorSampler : public TransitionSampler {
protected:
    virtual GlobalState sample(
        const GlobalState& state,
        const ProbabilisticOperator* op,
        const Distribution<GlobalState>& successors) override;

    distribution_random_sampler::DistributionRandomSampler sampler_;
};

} // namespace successor_sampler
} // namespace probabilistic

