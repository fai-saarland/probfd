#ifndef PROBFD_UTILS_TIMED_H
#define PROBFD_UTILS_TIMED_H

#include "downward/utils/timer.h"

#include <concepts>
#include <iostream>
#include <string>
#include <type_traits>

namespace probfd {

template <typename F, typename... Args>
std::invoke_result_t<F, Args...>
timed(std::ostream& out, const std::string& print, F&& f, Args&&... args)
    requires std::invocable<F, Args...>
{
    class Printer {
        utils::Timer timer;
        std::ostream& out;

    public:
        Printer(const std::string& print, std::ostream& out)
            : out(out)
        {
            out << print << ' ' << std::flush;
        }

        ~Printer()
        {
            if (std::uncaught_exceptions() > 0) {
                out << "Failed after " << timer() << '.' << std::endl;
            } else {
                out << "Finished after " << timer() << '.' << std::endl;
            }
        }
    };

    Printer _(print, out);
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}

}; // namespace probfd

#endif