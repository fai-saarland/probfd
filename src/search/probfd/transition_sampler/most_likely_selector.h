#ifndef MDPS_TRANSITION_SAMPLER_MOST_LIKELY_SELECTOR_H
#define MDPS_TRANSITION_SAMPLER_MOST_LIKELY_SELECTOR_H

#include "../transition_sampler.h"

namespace probfd {
namespace transition_sampler {

class MostLikelySuccessorSelector
    : public ProbabilisticOperatorTransitionSampler {
protected:
    virtual StateID sample(
        const StateID& state,
        const ProbabilisticOperator* op,
        const Distribution<StateID>& successors) override;
};

} // namespace transition_sampler
} // namespace probfd

#endif // __MOST_LIKELY_SELECTOR_H__