#ifndef MDPS_UTILS_DISTRIBUTION_RANDOM_SAMPLER_H
#define MDPS_UTILS_DISTRIBUTION_RANDOM_SAMPLER_H

#include "probfd/distribution.h"
#include "probfd/value_type.h"

#include "utils/rng.h"

#include <memory>

namespace probfd {
namespace distribution_random_sampler {

class DistributionRandomSampler {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    DistributionRandomSampler(std::shared_ptr<utils::RandomNumberGenerator> rng)
        : rng(rng)
    {
    }

    template <typename T>
    const T& operator()(const Distribution<T>& distribution)
    {
        assert(!distribution.empty());
        value_type::value_t p =
            std::min(value_type::from_double(rng->random()), value_type::one);
        assert(!value_type::is_approx_greater(p, value_type::one));
        assert(
            value_type::is_approx_greater(p, value_type::zero) ||
            value_type::is_approx_equal(p, value_type::zero));
        auto it = distribution.begin();
        while (true) {
            assert(it != distribution.end());
            p -= it->probability;
            if (!value_type::is_approx_greater(p, value_type::zero)) {
                break;
            }
            it++;
        }
        return it->element;
    }
};

} // namespace distribution_random_sampler
} // namespace probfd

#endif // __DISTRIBUTION_RANDOM_SAMPLER_H__