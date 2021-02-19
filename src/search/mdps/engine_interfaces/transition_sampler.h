#pragma once

#include "../distribution.h"
#include "../types.h"
#include "../utils/distribution_random_sampler.h"

namespace probabilistic {

template<typename Action>
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
