#ifndef UTILS_TIMER_H
#define UTILS_TIMER_H

#include <chrono>
#include <iosfwd>
#include <limits>
#include <numeric>

namespace downward::utils {

using Duration = std::chrono::duration<double>;

struct DynamicDuration {
    std::intmax_t num;
    std::intmax_t denom;
    long double value;

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
