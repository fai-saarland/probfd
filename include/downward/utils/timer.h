#ifndef UTILS_TIMER_H
#define UTILS_TIMER_H

#include <chrono>
#include <iosfwd>

namespace downward::utils {

using Duration = std::chrono::duration<double>;

class Timer {
    std::chrono::time_point<
        std::chrono::high_resolution_clock,
        std::chrono::duration<double>>
        last_start_clock;
    std::chrono::duration<double> collected_time;
    bool stopped;

    std::chrono::time_point<
        std::chrono::high_resolution_clock,
        std::chrono::duration<double>> current_clock() const;

public:
    explicit Timer(bool start = true);

    std::chrono::duration<double> operator()() const;
    std::chrono::duration<double> stop();
    void resume();
    std::chrono::duration<double> reset();
};

std::ostream& operator<<(std::ostream& os, const Timer& timer);

extern Timer g_search_timer;
extern Timer g_timer;
} // namespace downward::utils

#endif
