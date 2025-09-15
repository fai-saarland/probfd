#ifndef UTILS_TIMER_H
#define UTILS_TIMER_H

#include <chrono>
#include <iosfwd>

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

extern Timer g_search_timer;
extern Timer g_timer;
} // namespace downward::utils

#endif
