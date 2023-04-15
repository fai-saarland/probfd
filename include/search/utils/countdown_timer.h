#ifndef UTILS_COUNTDOWN_TIMER_H
#define UTILS_COUNTDOWN_TIMER_H

#include "utils/exceptions.h"
#include "utils/timer.h"

namespace utils {
class CountdownTimer {
    Timer timer;
    double max_time;

public:
    explicit CountdownTimer(double max_time);
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
