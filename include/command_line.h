#ifndef PROBFD_COMMAND_LINE_H
#define PROBFD_COMMAND_LINE_H

#include <memory>
#include <string>

// Forward Declarations
namespace probfd {
class TaskSolverFactory;
}

namespace probfd {

extern std::shared_ptr<TaskSolverFactory>
parse_cmd_line(int argc, const char** argv, bool is_unit_cost);

/// Returns a string documenting usage of the planner. Receives the
/// program name as input.
extern std::string usage(const std::string& progname);

} // namespace probfd

#endif
