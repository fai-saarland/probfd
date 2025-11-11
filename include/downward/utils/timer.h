#ifndef UTILS_TIMER_H
#define UTILS_TIMER_H

#include <chrono>
#include <iosfwd>
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

class Timer {
    std::chrono::time_point<std::chrono::high_resolution_clock, FSeconds>
        last_start_clock;
    FSeconds collected_time;
    bool stopped;

    std::chrono::time_point<std::chrono::high_resolution_clock, FSeconds>
    current_clock() const;

public:
    explicit Timer(bool start = true);

    FSeconds operator()() const;
    FSeconds stop();
    void resume();
    FSeconds reset();
};

std::ostream& operator<<(std::ostream& os, const Timer& timer);

} // namespace downward::utils

// Specialized so Duration::max() matches an infinite value.
template <>
struct std::chrono::duration_values<double> {
    static constexpr double zero() noexcept { return 0.0; }

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
