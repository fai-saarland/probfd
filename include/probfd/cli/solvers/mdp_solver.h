#ifndef PROBFD_PLUGINS_SOLVERS_MDP_SOLVER_H
#define PROBFD_PLUGINS_SOLVERS_MDP_SOLVER_H

#include "probfd/aliases.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

// Forward Declarations
class Evaluator;

namespace utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Options;
class Feature;
} // namespace downward::cli::plugins

namespace probfd {
class TaskEvaluatorFactory;
} // namespace probfd

/// This namespace contains the solver plugins for various search algorithms.
namespace probfd::cli::solvers {

extern void
add_base_solver_options_to_feature(downward::cli::plugins::Feature& feature);

extern std::tuple<
    utils::Verbosity,
    std::vector<std::shared_ptr<::Evaluator>>,
    bool,
    std::shared_ptr<probfd::TaskEvaluatorFactory>,
    std::optional<probfd::value_t>,
    bool,
    double,
    std::string,
    bool>
get_base_solver_args_from_options(
    const downward::cli::plugins::Options& options);

} // namespace probfd::cli::solvers

#endif