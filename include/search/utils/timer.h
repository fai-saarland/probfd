#ifndef UTILS_TIMER_H
#define UTILS_TIMER_H

#include <chrono>
#include <ostream>

namespace utils {
class Duration {
    std::chrono::duration<double> seconds;

public:
    explicit Duration(double seconds = 0)
        : seconds(seconds)
    {
    }

    explicit Duration(std::chrono::duration<double> seconds)
        : seconds(seconds)
    {
    }

    operator double() const { return seconds.count(); }

    Duration& operator+=(const Duration& other)
    {
        seconds += other.seconds;
        return *this;
    }

    Duration& operator-=(const Duration& other)
    {
        seconds -= other.seconds;
        return *this;
    }

    friend Duration operator+(const Duration& left, const Duration& right)
    {
        return Duration(left.seconds + right.seconds);
    }

    friend Duration operator-(const Duration& left, const Duration& right)
    {
        return Duration(left.seconds - right.seconds);
    }

    friend std::ostream& operator<<(std::ostream& os, const Duration& d)
    {
        return os << d.seconds.count() << "s";
    }
};

class Timer {
    using clock = std::chrono::high_resolution_clock;

    std::chrono::time_point<clock> last_start_clock;
    Duration collected_time;
    bool stopped;

public:
    Timer(bool stopped = false);
    ~Timer() = default;
    Duration operator()() const;
    Duration stop();
    void resume();
    Duration reset();

    friend std::ostream& operator<<(std::ostream& os, const Timer& timer)
    {
        return os << timer();
    }
};

extern Timer g_search_timer;
extern Timer g_timer;

class TimerScope {
    Timer& timer;

public:
    TimerScope(Timer& timer)
        : timer(timer)
    {
        timer.resume();
    }

    ~TimerScope() { timer.stop(); }
};

} // namespace utils

#endif
