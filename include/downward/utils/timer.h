#ifndef UTILS_TIMER_H
#define UTILS_TIMER_H

#include "downward/utils/durations.h"

#include <chrono>
#include <iosfwd>
#include <limits>
#include <numeric>

namespace downward::utils {

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

#endif
