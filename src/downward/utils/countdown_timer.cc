#include "downward/utils/countdown_timer.h"

#include <iostream>
#include <limits>

using namespace std;

namespace downward::utils {
CountdownTimer::CountdownTimer(double max_time)
    : max_time(max_time)
{
}

CountdownTimer::CountdownTimer(Duration max_time)
    : max_time(max_time)
{
}

CountdownTimer::~CountdownTimer()
{
}

bool CountdownTimer::is_expired() const
{
    /*
      We avoid querying the timer when it cannot expire so that we get cleaner
      output from "strace" (which otherwise reports the "times" system call
      millions of times.
    */
    return max_time != Duration::max() && timer() >= max_time;
}

void CountdownTimer::throw_if_expired() const
{
    if (is_expired()) { throw TimeoutException(); }
}

Duration CountdownTimer::get_elapsed_time() const
{
    return timer();
}

Duration CountdownTimer::get_remaining_time() const
{
    return Duration(max_time - get_elapsed_time());
}

ostream& operator<<(ostream& os, const CountdownTimer& cd_timer)
{
    os << cd_timer.timer;
    return os;
}
} // namespace downward::utils
