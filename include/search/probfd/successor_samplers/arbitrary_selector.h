#ifndef PROBFD_SUCCESSOR_SAMPLERS_ARBITRARY_SELECTOR_H
#define PROBFD_SUCCESSOR_SAMPLERS_ARBITRARY_SELECTOR_H

#include "probfd/engine_interfaces/successor_sampler.h"

#include "operator_id.h"

namespace probfd {

/// This namespace contains implementations of transition successor samplers.
namespace successor_samplers {

class ArbitrarySuccessorSelector : public TaskSuccessorSampler {
protected:
    StateID sample(
        StateID state,
        OperatorID op,
        const Distribution<StateID>& successors,
        engine_interfaces::HeuristicSearchInterface<OperatorID>& hs_interface)
        override;
};

} // namespace successor_samplers
} // namespace probfd

#endif // __ARBITRARY_SELECTOR_H__