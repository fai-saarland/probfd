#ifndef PROBFD_TRANSITION_SAMPLERS_ARBITRARY_SELECTOR_H
#define PROBFD_TRANSITION_SAMPLERS_ARBITRARY_SELECTOR_H

#include "probfd/engine_interfaces/transition_sampler.h"

#include "operator_id.h"

namespace probfd {

/// This namespace contains implementations of transition successor samplers.
namespace transition_samplers {

class ArbitrarySuccessorSelector : public TaskTransitionSampler {
protected:
    StateID sample(
        StateID state,
        OperatorID op,
        const Distribution<StateID>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface) override;
};

} // namespace transition_samplers
} // namespace probfd

#endif // __ARBITRARY_SELECTOR_H__