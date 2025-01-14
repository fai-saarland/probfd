#ifndef PROBFD_PLUGINS_SOLVERS_MDP_SOLVER_OPTIONS_FWD_H
#define PROBFD_PLUGINS_SOLVERS_MDP_SOLVER_OPTIONS_FWD_H

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
class TaskEvaluatorFactory;
} // namespace probfd

namespace probfd::cli::solvers {

using MDPSolverAddditionalArgs = std::tuple<
    std::vector<std::shared_ptr<::Evaluator>>,
    bool,
    std::shared_ptr<probfd::TaskEvaluatorFactory>,
    std::optional<probfd::value_t>,
    bool,
    double,
    std::string,
    bool>;

using MDPSolverArgs =
    tuple_cat_t<std::tuple<utils::Verbosity>, MDPSolverAddditionalArgs>;

} // namespace probfd::cli::solvers

#endif
