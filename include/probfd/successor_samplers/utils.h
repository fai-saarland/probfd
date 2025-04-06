
#ifndef PROBFD_SUCCESSOR_SAMPLERS_UTILS_H
#define PROBFD_SUCCESSOR_SAMPLERS_UTILS_H

#include "probfd/item_probability_pair.h"

#include "downward/utils/rng.h"

#include <ranges>

namespace probfd::successor_samplers {

template <std::ranges::input_range R>
    requires Specialization<std::ranges::range_value_t<R>, ItemProbabilityPair>
std::ranges::iterator_t<R> weighted_select(
    R&& range,
    value_t total_weight,
    downward::utils::RandomNumberGenerator& rng)
{
    const value_t r = total_weight * rng.random();

    auto it = std::ranges::begin(range);
    value_t s = it->probability;

    while (s <= r) {
        s += (++it)->probability;
    }

    return it;
}

} // namespace probfd::successor_samplers

#endif // PROBFD_SUCCESSOR_SAMPLERS_UTILS_H
