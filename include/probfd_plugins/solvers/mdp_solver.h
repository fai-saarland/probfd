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

namespace downward_plugins::plugins {
class Options;
class Feature;
} // namespace downward_plugins::plugins

namespace probfd {
class TaskEvaluatorFactory;
} // namespace probfd

/// This namespace contains the solver plugins for various search algorithms.
namespace probfd_plugins::solvers {

extern void
add_base_solver_options_to_feature(downward_plugins::plugins::Feature& feature);

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
    const downward_plugins::plugins::Options& options);

} // namespace probfd_plugins::solvers

#endif