#ifndef PROBFD_TRANSITION_SAMPLERS_MOST_LIKELY_SELECTOR_H
#define PROBFD_TRANSITION_SAMPLERS_MOST_LIKELY_SELECTOR_H

#include "probfd/engine_interfaces/transition_sampler.h"

namespace probfd {
namespace transition_samplers {

class MostLikelySuccessorSelector : public TaskTransitionSampler {
protected:
    virtual StateID sample(
        const StateID& state,
        const OperatorID& op,
        const Distribution<StateID>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface) override;
};

} // namespace transition_samplers
} // namespace probfd

#endif // __MOST_LIKELY_SELECTOR_H__