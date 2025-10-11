#ifndef UTILS_SYSTEM_H
#define UTILS_SYSTEM_H

#define LINUX 0
#define OSX 1
#define WINDOWS 2

#if defined(_WIN32)
#define OPERATING_SYSTEM WINDOWS
#include "downward/utils/system_windows.h"
#elif defined(__APPLE__)
#define OPERATING_SYSTEM OSX
#include "downward/utils/system_unix.h"
#else
#define OPERATING_SYSTEM LINUX
#include "downward/utils/system_unix.h"
#endif

#include "downward/utils/units.h"

#include <iostream>
#include <source_location>
#include <stdlib.h>

namespace downward::utils {
enum class ExitCode {
    /*
      For a full list of exit codes, please see driver/returncodes.py. Here,
      we only list codes that are used by the search component of the planner.
    */
    // 0-9: exit codes denoting a plan was found
    SUCCESS = 0,

    // 10-19: exit codes denoting no plan was found (without any error)
    SEARCH_UNSOLVABLE = 11, // Task is provably unsolvable with given bound.
    SEARCH_UNSOLVED_INCOMPLETE = 12, // Search ended without finding a solution.

    // 20-29: "expected" failures
    SEARCH_OUT_OF_MEMORY = 22,
    SEARCH_OUT_OF_TIME = 23,

    // 30-39: unrecoverable errors
    SEARCH_CRITICAL_ERROR = 32,
    SEARCH_INPUT_ERROR = 33,
    SEARCH_UNSUPPORTED = 34,
    SEARCH_UNIMPLEMENTED = 35
};

class CriticalError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    template <class... Args>
    explicit CriticalError(std::format_string<Args...> fmt, Args&&... args)
        : std::runtime_error(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

class InputError : public std::domain_error {
public:
    using std::domain_error::domain_error;

    template <class... Args>
    explicit InputError(std::format_string<Args...> fmt, Args&&... args)
        : std::domain_error(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

class UnsupportedError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    template <class... Args>
    explicit UnsupportedError(std::format_string<Args...> fmt, Args&&... args)
        : std::runtime_error(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

class UnimplementedError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    template <class... Args>
    explicit UnimplementedError(std::format_string<Args...> fmt, Args&&... args)
        : std::runtime_error(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

class OverflowError : public std::overflow_error {
public:
    using std::overflow_error::overflow_error;

    template <class... Args>
    explicit OverflowError(std::format_string<Args...> fmt, Args&&... args)
        : std::overflow_error(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

[[noreturn]]
extern void exit_with_reentrant(ExitCode returncode);

Kilobytes get_peak_memory_in_kb();
const char* get_exit_code_message_reentrant(ExitCode exitcode);
bool is_exit_code_error_reentrant(ExitCode exitcode);
void register_event_handlers();
void report_exit_code_reentrant(ExitCode exitcode);
int get_process_id();
} // namespace downward::utils

#endif
