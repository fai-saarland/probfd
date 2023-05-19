
#ifndef PROBFD_UTILS_GUARDS_H
#define PROBFD_UTILS_GUARDS_H

#include <exception>

namespace utils {
class Timer;
}

namespace probfd {

template <typename F>
class scope_success {
    F f;

public:
    scope_success(F&& f)
        : f(std::move(f))
    {
    }
    ~scope_success()
    {
        if (!std::uncaught_exceptions()) f();
    }
};

template <typename F>
class scope_fail {
    F f;

public:
    scope_fail(F&& f)
        : f(std::move(f))
    {
    }
    ~scope_fail()
    {
        if (std::uncaught_exceptions()) f();
    }
};

template <typename F>
class scope_exit {
    F f;

public:
    scope_exit(F&& f)
        : f(std::move(f))
    {
    }
    ~scope_exit() { f(); }
};

class TimerScope {
    utils::Timer& timer;

public:
    TimerScope(utils::Timer& timer);
    ~TimerScope();
};

} // namespace probfd

#endif