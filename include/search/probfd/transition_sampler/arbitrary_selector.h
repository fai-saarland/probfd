#ifndef PROBFD_TRANSITION_SAMPLER_ARBITRARY_SELECTOR_H
#define PROBFD_TRANSITION_SAMPLER_ARBITRARY_SELECTOR_H

#include "probfd/engine_interfaces/transition_sampler.h"

namespace probfd {

/// Namespace dedicated to transition sampling.
namespace transition_sampler {

class ArbitrarySuccessorSelector : public TaskTransitionSampler {
protected:
    virtual StateID sample(
        const StateID& state,
        const OperatorID& op,
        const Distribution<StateID>& successors) override;
};

} // namespace transition_sampler
} // namespace probfd

#endif // __ARBITRARY_SELECTOR_H__