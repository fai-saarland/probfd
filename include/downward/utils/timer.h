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

struct DynamicDuration {
    std::intmax_t num;
    std::intmax_t denom;
    long double value;

    DynamicDuration() = default;

    DynamicDuration(std::intmax_t num, std::intmax_t denom, long double value)
        : num(num)
        , denom(denom)
        , value(value)
    {
    }

    template <typename Rep, typename Period>
    DynamicDuration(std::chrono::duration<Rep, Period> d)
        : num(Period::num)
        , denom(Period::den)
        , value(static_cast<long double>(d.count()))
    {
    }

    template <typename Rep, typename Period>
    explicit operator std::chrono::duration<Rep, Period>() const
    {
        const intmax_t gx = std::gcd(num, Period::num);
        const intmax_t gy = std::gcd(Period::den, denom);

        std::uintmax_t rnum = (num / gx) * (Period::den / gy);
        std::uintmax_t rden = (denom / gy) * (Period::num / gx);

        using CR = std::common_type_t<Rep, long double, intmax_t>;

        if (rden == 1) {
            if (rnum == 1) {
                return static_cast<std::chrono::duration<Rep, Period>>(
                    static_cast<Rep>(value));
            } else {
                return static_cast<std::chrono::duration<Rep, Period>>(
                    static_cast<Rep>(
                        static_cast<CR>(value) * static_cast<CR>(rnum)));
            }
        } else {
            if (rnum == 1) {
                return static_cast<std::chrono::duration<Rep, Period>>(
                    static_cast<Rep>(
                        static_cast<CR>(value) / static_cast<CR>(rden)));
            } else {
                return static_cast<std::chrono::duration<Rep, Period>>(
                    static_cast<Rep>(
                        static_cast<CR>(value) * static_cast<CR>(rnum) /
                        static_cast<CR>(rden)));
            }
        }
    }
};

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
