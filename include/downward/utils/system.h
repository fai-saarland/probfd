#ifndef UTILS_SYSTEM_H
#define UTILS_SYSTEM_H

#include "downward/utils/byte_units.h"

#define LINUX 0
#define OSX 1
#define WINDOWS 2

#if defined(_WIN32)
#define OPERATING_SYSTEM WINDOWS
#elif defined(__APPLE__)
#define OPERATING_SYSTEM OSX
#else
#define OPERATING_SYSTEM LINUX
#endif

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

[[noreturn]]
extern void exit_with_reentrant(ExitCode returncode);

Kibibytes get_peak_memory_in_kib();
const char* get_exit_code_message_reentrant(ExitCode exitcode);
bool is_exit_code_error_reentrant(ExitCode exitcode);
void register_event_handlers();
void report_exit_code_reentrant(ExitCode exitcode);
int get_process_id();
} // namespace downward::utils

#endif
