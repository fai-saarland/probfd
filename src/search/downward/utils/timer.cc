#include "downward/utils/timer.h"

namespace utils {

Timer::Timer(bool stop)
    : last_start_clock(clock::now())
    , stopped(stop)
{
}

Duration Timer::stop()
{
    collected_time = (*this)();
    stopped = true;
    return collected_time;
}

Duration Timer::operator()() const
{
    if (stopped) {
        return collected_time;
    }

    return collected_time + Duration(clock::now() - last_start_clock);
}

void Timer::resume()
{
    if (stopped) {
        stopped = false;
        last_start_clock = clock::now();
    }
}

Duration Timer::reset()
{
    const Duration result = (*this)();
    collected_time = Duration(0);
    last_start_clock = clock::now();
    return result;
}

Timer g_search_timer(true);
Timer g_timer;
} // namespace utils
