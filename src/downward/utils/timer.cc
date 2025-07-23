#include "downward/utils/timer.h"

#include <ostream>

using namespace std;

namespace downward::utils {

static std::chrono::duration<double> compute_sanitized_duration(
    std::chrono::time_point<
        std::chrono::high_resolution_clock,
        std::chrono::duration<double>> start_clock,
    std::chrono::time_point<
        std::chrono::high_resolution_clock,
        std::chrono::duration<double>> end_clock)
{
    /*
        Sometimes we measure durations that are closer to 0 than should be
       physically possible with measurements on a single CPU. Note that with a
       CPU frequency less than 10 GHz, each clock cycle will take more than
       1e-10 seconds. Even worse, these close-to-zero durations are sometimes
       negative. We sanitize them to 0.
    */
    std::chrono::duration<double> duration = end_clock - start_clock;
    if (duration.count() > -1e-10 && duration.count() < 1e-10) duration = 0s;
    return duration;
}

Timer::Timer(bool start)
{
    collected_time = 0s;
    stopped = !start;
    if (start)
        last_start_clock = current_clock();
    else
        last_start_clock = std::chrono::time_point<
            std::chrono::high_resolution_clock,
            std::chrono::duration<double>>::min();
}

std::chrono::time_point<
    std::chrono::high_resolution_clock,
    std::chrono::duration<double>>
Timer::current_clock() const
{
    return std::chrono::high_resolution_clock::now();
}

std::chrono::duration<double> Timer::stop()
{
    collected_time = (*this)();
    stopped = true;
    return collected_time;
}

std::chrono::duration<double> Timer::operator()() const
{
    if (stopped)
        return collected_time;
    else
        return collected_time +
               compute_sanitized_duration(last_start_clock, current_clock());
}

void Timer::resume()
{
    if (stopped) {
        stopped = false;
        last_start_clock = current_clock();
    }
}

std::chrono::duration<double> Timer::reset()
{
    const std::chrono::duration<double> result = (*this)();
    collected_time = 0s;
    last_start_clock = current_clock();
    return result;
}

ostream& operator<<(ostream& os, const Timer& timer)
{
    os << timer();
    return os;
}

Timer g_search_timer(false);
Timer g_timer;
} // namespace downward::utils
