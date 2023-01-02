#ifndef MDPS_UTILS_DISTRIBUTION_UNIFORM_SAMPLER_H
#define MDPS_UTILS_DISTRIBUTION_UNIFORM_SAMPLER_H

#include "probfd/distribution.h"

#include "utils/rng.h"

namespace probfd {
namespace distribution_uniform_sampler {

class DistributionUniformSampler {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    DistributionUniformSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng)
        : rng(rng)
    {
    }

    template <typename T>
    const T& operator()(const Distribution<T>& distribution)
    {
        assert(!distribution.empty());
        unsigned selection = rng->random(distribution.size());
        auto it = distribution.begin();
        for (; selection > 0; it++) {
            assert(it != distribution.end());
            selection--;
        }
        assert(it != distribution.end());
        return it->element;
    }
};

} // namespace distribution_uniform_sampler
} // namespace probfd

#endif // __DISTRIBUTION_UNIFORM_SAMPLER_H__