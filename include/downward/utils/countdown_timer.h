#ifndef UTILS_COUNTDOWN_TIMER_H
#define UTILS_COUNTDOWN_TIMER_H

#include "downward/utils/exceptions.h"
#include "downward/utils/timer.h"

namespace downward::utils {
class CountdownTimer {
    Timer timer;
    Duration max_time;

public:
    explicit CountdownTimer(Duration max_time);
    ~CountdownTimer();

    bool is_expired() const;
    void throw_if_expired() const;
    Duration get_elapsed_time() const;
    Duration get_remaining_time() const;
    friend std::ostream&
    operator<<(std::ostream& os, const CountdownTimer& cd_timer);
};

std::ostream& operator<<(std::ostream& os, const CountdownTimer& cd_timer);
} // namespace utils

#endif
