#pragma once

#include "../../globals.h"
#include "../../utils/rng.h"
#include "../distribution.h"

namespace probabilistic {
namespace distribution_uniform_sampler {

class DistributionUniformSampler {
public:
    DistributionUniformSampler();
    ~DistributionUniformSampler() = default;

    template<typename T>
    const T& operator()(const Distribution<T>& distribution) const
    {
        assert(!distribution.empty());
        unsigned selection = g_rng(distribution.size());
        auto it = distribution.begin();
        for (; selection > 0; it++) {
            assert(it != distribution.end());
            selection--;
        }
        assert(it != distribution.end());
        return it->first;
    }
};

} // namespace distribution_uniform_sampler
} // namespace probabilistic
