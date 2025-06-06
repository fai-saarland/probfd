#include "probfd/utils/guards.h"

#include "downward/utils/timer.h"

namespace probfd {

TimerScope::TimerScope(downward::utils::Timer& timer)
    : timer_(timer)
{
    timer.resume();
}

TimerScope::~TimerScope()
{
    timer_.stop();
}

} // namespace probfd