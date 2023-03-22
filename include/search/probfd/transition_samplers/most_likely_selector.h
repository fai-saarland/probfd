#ifndef PROBFD_TRANSITION_SAMPLERS_MOST_LIKELY_SELECTOR_H
#define PROBFD_TRANSITION_SAMPLERS_MOST_LIKELY_SELECTOR_H

#include "probfd/engine_interfaces/transition_sampler.h"

#include "operator_id.h"

namespace probfd {
namespace transition_samplers {

class MostLikelySuccessorSelector : public TaskTransitionSampler {
protected:
    StateID sample(
        StateID state,
        OperatorID op,
        const Distribution<StateID>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface) override;
};

} // namespace transition_samplers
} // namespace probfd

#endif // __MOST_LIKELY_SELECTOR_H__