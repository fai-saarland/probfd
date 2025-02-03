#ifndef PROBFD_CLI_SOLVERS_MDP_SOLVER_OPTIONS_FWD_H
#define PROBFD_CLI_SOLVERS_MDP_SOLVER_OPTIONS_FWD_H

#include "probfd/aliases.h"
#include "probfd/type_traits.h"

#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

// Forward Declarations
class Evaluator;

namespace utils {
enum class Verbosity;
}

namespace probfd {
class TaskStateSpaceFactory;
class TaskHeuristicFactory;
} // namespace probfd

namespace probfd::cli::solvers {

using MDPSolverAddditionalArgs = std::tuple<
    std::shared_ptr<probfd::TaskStateSpaceFactory>,
    std::shared_ptr<probfd::TaskHeuristicFactory>,
    std::string,
    bool,
    std::optional<probfd::value_t>,
    bool>;

using MDPSolverArgs =
    tuple_cat_t<std::tuple<utils::Verbosity>, MDPSolverAddditionalArgs>;

} // namespace probfd::cli::solvers

#endif
