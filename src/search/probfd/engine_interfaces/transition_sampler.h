#ifndef MDPS_ENGINE_INTERACES_TRANSITION_SAMPLER_H
#define MDPS_ENGINE_INTERACES_TRANSITION_SAMPLER_H

#include "probfd/utils/distribution_random_sampler.h"

#include "probfd/distribution.h"
#include "probfd/types.h"

namespace probfd {
namespace engine_interfaces {

/**
 * @brief Function object used to sample a possible successor state for the
 * given source state, applied action and successor distribution.
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename Action>
struct TransitionSampler {
    StateID operator()(
        const StateID&,
        const Action&,
        const Distribution<StateID>& transition)
    {
        return distribution_random_sampler::DistributionRandomSampler()(
            transition);
    }
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __TRANSITION_SAMPLER_H__