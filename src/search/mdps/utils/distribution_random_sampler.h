#ifndef MDPS_UTILS_DISTRIBUTION_RANDOM_SAMPLER_H
#define MDPS_UTILS_DISTRIBUTION_RANDOM_SAMPLER_H

#include "../../globals.h"
#include "../../utils/rng.h"
#include "../distribution.h"
#include "../value_type.h"

namespace probabilistic {
namespace distribution_random_sampler {

class DistributionRandomSampler {
public:
    DistributionRandomSampler();
    ~DistributionRandomSampler() = default;

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
            p -= it->second;
            if (!value_type::approx_greater()(p, value_type::zero)) {
                break;
            }
            it++;
        }
        return it->first;
    }

private:
};

} // namespace distribution_random_sampler
} // namespace probabilistic

#endif // __DISTRIBUTION_RANDOM_SAMPLER_H__