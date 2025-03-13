#ifndef PROBFD_CLI_SOLVERS_MDP_SOLVER_OPTIONS_FWD_H
#define PROBFD_CLI_SOLVERS_MDP_SOLVER_OPTIONS_FWD_H

#include "probfd/aliases.h"

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

namespace probfd::solvers {
class StatisticalMDPAlgorithmFactory;
} // namespace probfd::solvers

namespace probfd::cli::solvers {

using MDPSolverNoAlgorithmArgs = std::tuple<
    std::shared_ptr<probfd::TaskStateSpaceFactory>,
    std::shared_ptr<probfd::TaskHeuristicFactory>,
    utils::Verbosity,
    std::string,
    bool,
    std::optional<probfd::value_t>,
    bool>;

using MDPSolverArgs = TupleCatType<
    std::tuple<
        std::shared_ptr<probfd::solvers::StatisticalMDPAlgorithmFactory>>,
    MDPSolverNoAlgorithmArgs>;

} // namespace probfd::cli::solvers

#endif
