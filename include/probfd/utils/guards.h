
#ifndef PROBFD_UTILS_GUARDS_H
#define PROBFD_UTILS_GUARDS_H

#include <exception>
#include <utility>

namespace downward::utils {
class Timer;
}

namespace probfd {

template <typename F>
class scope_success {
    F f_;

public:
    explicit scope_success(F&& f)
        : f_(std::move(f))
    {
    }
    ~scope_success()
    {
        if (!std::uncaught_exceptions()) f_();
    }
};

template <typename F>
class scope_fail {
    F f_;

public:
    explicit scope_fail(F&& f)
        : f_(std::move(f))
    {
    }
    ~scope_fail()
    {
        if (std::uncaught_exceptions()) f_();
    }
};

template <typename F>
class scope_exit {
    F f_;

public:
    explicit scope_exit(F&& f)
        : f_(std::move(f))
    {
    }
    ~scope_exit() { f_(); }
};

class TimerScope {
    downward::utils::Timer& timer_;

public:
    explicit TimerScope(downward::utils::Timer& timer);
    ~TimerScope();
};

} // namespace probfd

#endif