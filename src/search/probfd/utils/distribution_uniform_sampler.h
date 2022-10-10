#ifndef MDPS_UTILS_DISTRIBUTION_UNIFORM_SAMPLER_H
#define MDPS_UTILS_DISTRIBUTION_UNIFORM_SAMPLER_H

#include "../../globals.h"
#include "../../utils/rng.h"
#include "../distribution.h"

namespace probabilistic {
namespace distribution_uniform_sampler {

struct DistributionUniformSampler {
    template <typename T>
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

#endif // __DISTRIBUTION_UNIFORM_SAMPLER_H__