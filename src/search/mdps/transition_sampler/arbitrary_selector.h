#pragma once

#include "../transition_sampler.h"

namespace probabilistic {

/// Namespace dedicated to transition sampling.
namespace transition_sampler {

class ArbitrarySuccessorSelector
    : public ProbabilisticOperatorTransitionSampler {
protected:
    virtual StateID sample(
        const StateID& state,
        const ProbabilisticOperator* op,
        const Distribution<StateID>& successors) override;
};

} // namespace transition_sampler
} // namespace probabilistic
