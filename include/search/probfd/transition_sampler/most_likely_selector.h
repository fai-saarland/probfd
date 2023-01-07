#ifndef PROBFD_TRANSITION_SAMPLER_MOST_LIKELY_SELECTOR_H
#define PROBFD_TRANSITION_SAMPLER_MOST_LIKELY_SELECTOR_H

#include "probfd/engine_interfaces/transition_sampler.h"

namespace probfd {
namespace transition_sampler {

class MostLikelySuccessorSelector : public TaskTransitionSampler {
protected:
    virtual StateID sample(
        const StateID& state,
        const OperatorID& op,
        const Distribution<StateID>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface) override;
};

} // namespace transition_sampler
} // namespace probfd

#endif // __MOST_LIKELY_SELECTOR_H__