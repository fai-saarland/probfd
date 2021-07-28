#pragma once

#include "../distribution.h"
#include "../types.h"
#include "../utils/distribution_random_sampler.h"

namespace probabilistic {

/**
 * @brief Function object used to sample a possible successor state for the
 * given source state, applied action and successor distribution.
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename Action>
struct TransitionSampler {
    using is_default_implementation = std::true_type;

    StateID operator()(
        const StateID&,
        const Action&,
        const Distribution<StateID>& transition)
    {
        return distribution_random_sampler::DistributionRandomSampler()(
            transition);
    }
};

} // namespace probabilistic
