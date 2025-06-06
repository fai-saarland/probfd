#ifndef PROBFD_UTILS_TIMED_H
#define PROBFD_UTILS_TIMED_H

#include "downward/utils/timer.h"

#include <concepts>
#include <iostream>
#include <string>
#include <type_traits>

namespace probfd {

class PrintingTimer {
    downward::utils::Timer& timer;
    std::string print;
    std::ostream& out;
    bool same_line;

public:
    PrintingTimer(
        downward::utils::Timer& timer,
        std::string print,
        std::ostream& out,
        bool same_line = true)
        : timer(timer)
        , print(std::move(print))
        , out(out)
        , same_line(same_line)
    {
        if (same_line) {
            out << this->print << ' ' << std::flush;
        } else {
            out << this->print << std::endl;
        }
    }

    ~PrintingTimer()
    {
        if (!same_line) {
            out << print << ' ';
        }

        if (std::uncaught_exceptions() > 0) {
            out << "Failed after " << timer.stop() << '.' << std::endl;
        } else {
            out << "Finished after " << timer.stop() << '.' << std::endl;
        }
    }
};

template <typename F, typename... Args>
std::invoke_result_t<F, Args...> run_time_logged(
    std::ostream& out,
    const std::string& print,
    bool same_line,
    F&& f,
    Args&&... args)
    requires std::invocable<F, Args...>
{
    downward::utils::Timer timer;
    PrintingTimer _(timer, print, out, same_line);
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}

template <typename F, typename... Args>
std::invoke_result_t<F, Args...> run_time_logged(
    std::ostream& out,
    const std::string& print,
    F&& f,
    Args&&... args)
    requires std::invocable<F, Args...>
{
    return run_time_logged(
        out,
        print,
        true,
        std::forward<F>(f),
        std::forward<Args>(args)...);
}

template <typename F, typename... Args>
std::invoke_result_t<F, Args...> run_time_logged(
    downward::utils::Timer& timer,
    std::ostream& out,
    const std::string& print,
    bool same_line,
    F&& f,
    Args&&... args)
    requires std::invocable<F, Args...>
{
    PrintingTimer _(timer, print, out, same_line);
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}

template <typename F, typename... Args>
std::invoke_result_t<F, Args...> run_time_logged(
    downward::utils::Timer& timer,
    std::ostream& out,
    const std::string& print,
    F&& f,
    Args&&... args)
    requires std::invocable<F, Args...>
{
    PrintingTimer _(timer, print, out);
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}


}; // namespace probfd

#endif