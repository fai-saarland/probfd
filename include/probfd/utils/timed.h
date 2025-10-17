#ifndef PROBFD_UTILS_TIMED_H
#define PROBFD_UTILS_TIMED_H

#include "downward/utils/timer.h"

#include <concepts>
#include <iostream>
#include <string>
#include <type_traits>

namespace probfd {

class PrintingTimer {
    std::ostream& out;
    std::format_string<double> on_fail;
    std::format_string<double> on_success;

    downward::utils::Timer timer;

public:
    explicit PrintingTimer(std::ostream& out)
        : out(out)
        , on_fail("Failed after {:.3f} seconds.")
        , on_success("Finished after {:.3f} seconds.")
    {
    }

    PrintingTimer(
        std::ostream& out,
        std::format_string<double> on_fail,
        std::format_string<double> on_success)
        : out(out)
        , on_fail(std::move(on_fail))
        , on_success(std::move(on_success))
    {
    }

    ~PrintingTimer()
    {
        const downward::utils::FSeconds d = timer.stop();

        if (std::uncaught_exceptions() > 0) {
            std::println(out, on_fail, d.count());
        } else {
            std::println(out, on_success, d.count());
        }
    }

    const downward::utils::Timer& get_timer() const { return timer; }
};

template <typename F, typename... Args>
std::invoke_result_t<F, Args...>
run_log_time(std::ostream& out, F&& f, Args&&... args)
    requires std::invocable<F, Args...>
{
    PrintingTimer timer(out);
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}

template <typename F, typename... Args>
std::invoke_result_t<F, const downward::utils::Timer&, Args...>
run_log_time(std::ostream& out, F&& f, Args&&... args)
    requires std::invocable<F, const downward::utils::Timer&, Args...>
{
    PrintingTimer timer(out);
    return std::invoke(
        std::forward<F>(f),
        timer.get_timer(),
        std::forward<Args>(args)...);
}

template <typename F, typename... Args>
std::invoke_result_t<F, Args...> run_log_when_done(
    std::ostream& out,
    std::format_string<double> on_done,
    F&& f,
    Args&&... args)
    requires std::invocable<F, Args...>
{
    PrintingTimer timer(out, on_done, on_done);
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}

template <typename F, typename... Args>
std::invoke_result_t<F, const downward::utils::Timer&, Args...>
run_log_when_done(
    std::ostream& out,
    std::format_string<double> on_done,
    F&& f,
    Args&&... args)
    requires std::invocable<F, const downward::utils::Timer&, Args...>
{
    PrintingTimer timer(out, on_done, on_done);
    return std::invoke(
        std::forward<F>(f),
        timer.get_timer(),
        std::forward<Args>(args)...);
}

template <typename F, typename... Args>
std::pair<std::invoke_result_t<F, Args...>, downward::utils::FSeconds>
run_log_time_r(std::ostream& out, F&& f, Args&&... args)
    requires std::invocable<F, Args...>
{
    PrintingTimer timer(out);
    auto r = std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    return std::make_pair(r, timer.get_timer().operator()());
}

template <typename F, typename... Args>
std::pair<
    std::invoke_result_t<F, const downward::utils::Timer&, Args...>,
    downward::utils::FSeconds>
run_log_time_r(std::ostream& out, F&& f, Args&&... args)
    requires std::invocable<F, const downward::utils::Timer&, Args...>
{
    PrintingTimer timer(out);
    auto r = std::invoke(
        std::forward<F>(f),
        timer.get_timer(),
        std::forward<Args>(args)...);
    return std::make_pair(std::move(r), timer.get_timer().operator()());
}

}; // namespace probfd

#endif