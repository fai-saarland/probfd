#ifndef UTILS_TIMER_H
#define UTILS_TIMER_H

#include <chrono>
#include <iosfwd>
#include <limits>

namespace downward::utils {

using Duration = std::chrono::duration<double>;

class Timer {
    std::chrono::time_point<std::chrono::high_resolution_clock, Duration>
        last_start_clock;
    Duration collected_time;
    bool stopped;

    std::chrono::time_point<std::chrono::high_resolution_clock, Duration>
    current_clock() const;

public:
    explicit Timer(bool start = true);

    Duration operator()() const;
    Duration stop();
    void resume();
    Duration reset();
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
