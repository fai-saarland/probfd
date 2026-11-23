#ifndef PROBFD_HEURISTICS_UTILS_H
#define PROBFD_HEURISTICS_UTILS_H

#include "probfd/value_type.h"

namespace probfd::heuristics {

value_t sum_estimate(auto&& estimation_range)
{
    value_t value = 0_vt;

    for (const auto& estimation : estimation_range) {
        if (estimation == INFINITE_VALUE) {
            return INFINITE_VALUE;
        }

        value += estimation;
    }

    return value;
}

value_t max_estimate(auto&& estimation_range)
{
    value_t value = 0_vt;

    for (const auto& estimation : estimation_range) {
        if (estimation == INFINITE_VALUE) {
            return INFINITE_VALUE;
        }

        value = std::max(value, estimation);
    }

    return value;
}

} // namespace probfd::heuristics

#endif
