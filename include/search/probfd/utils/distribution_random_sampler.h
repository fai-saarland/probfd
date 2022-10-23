#ifndef MDPS_UTILS_DISTRIBUTION_RANDOM_SAMPLER_H
#define MDPS_UTILS_DISTRIBUTION_RANDOM_SAMPLER_H

#include "probfd/distribution.h"
#include "probfd/value_type.h"

#include "utils/rng.h"

#include "globals.h"

namespace probfd {
namespace distribution_random_sampler {

struct DistributionRandomSampler {
    template <typename T>
    const T& operator()(const Distribution<T>& distribution) const
    {
        assert(!distribution.empty());
        value_type::value_t p =
            value_type::cap(value_type::from_double(g_rng()), value_type::one);
        assert(!value_type::approx_greater()(p, value_type::one));
        assert(
            value_type::approx_greater()(p, value_type::zero) ||
            value_type::approx_equal()(p, value_type::zero));
        auto it = distribution.begin();
        while (true) {
            assert(it != distribution.end());
            p -= it->probability;
            if (!value_type::approx_greater()(p, value_type::zero)) {
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