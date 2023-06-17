#include "probfd/utils/guards.h"

#include "downward/utils/timer.h"

namespace probfd {

TimerScope::TimerScope(utils::Timer& timer)
    : timer(timer)
{
    timer.resume();
}

TimerScope::~TimerScope()
{
    timer.stop();
}

} // namespace probfd