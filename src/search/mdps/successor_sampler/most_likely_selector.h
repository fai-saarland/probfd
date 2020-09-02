#pragma once

#include "../../global_state.h"
#include "../transition_sampler.h"

namespace probabilistic {
namespace successor_sampler {

class MostLikelySuccessorSelector : public TransitionSampler {
protected:
    virtual GlobalState sample(
        const GlobalState& state,
        const ProbabilisticOperator* op,
        const Distribution<GlobalState>& successors) override;
};

} // namespace successor_sampler
} // namespace probabilistic
