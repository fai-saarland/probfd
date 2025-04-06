#ifndef PROBFD_CLI_HEURISTICS_TASK_DEPENDENT_HEURISTIC_OPTIONS_FWD_H
#define PROBFD_CLI_HEURISTICS_TASK_DEPENDENT_HEURISTIC_OPTIONS_FWD_H

#include <tuple>

// Forward Declarations
namespace downward::utils {
enum class Verbosity;
}

namespace probfd::cli::heuristics {
using TaskDependentHeuristicArgs = std::tuple<downward::utils::Verbosity>;
} // namespace probfd::cli::heuristics

#endif // PROBFD_CLI_HEURISTICS_TASK_DEPENDENT_HEURISTIC_OPTIONS_FWD_H
