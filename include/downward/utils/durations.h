#ifndef UTILS_DURATIONS_H
#define UTILS_DURATIONS_H

#include <chrono>
#include <limits>
#include <numeric>

namespace downward::utils {

template <typename T>
using FDuration = std::chrono::duration<double, T>;

using FHours = FDuration<std::ratio<60 * 60>>;
using FMinutes = FDuration<std::ratio<60>>;
using FSeconds = FDuration<std::ratio<1>>;
using FMilliSeconds = FDuration<std::milli>;
using FMicroSeconds = FDuration<std::micro>;
using FNanoSeconds = FDuration<std::nano>;

} // namespace downward::utils

// Specialized so Duration::max() matches an infinite value.
template <>
struct std::chrono::duration_values<double> {
    static constexpr double zero() noexcept
    {
        return 0.0;
    }

    static constexpr double max() noexcept
    {
        return std::numeric_limits<double>::infinity();
    }

    static constexpr double min() noexcept
    {
        return -std::numeric_limits<double>::infinity();
    }
};

#endif
