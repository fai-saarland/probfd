#ifndef PROBFD_PLUGINS_SOLVERS_MDP_SOLVER_H
#define PROBFD_PLUGINS_SOLVERS_MDP_SOLVER_H

#include "probfd/value_type.h"

#include "downward/utils/logging.h"

#include <memory>
#include <optional>
#include <string>

// Forward Declarations
class Evaluator;

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
class TaskCostFunctionFactory;
class TaskEvaluatorFactory;
} // namespace probfd

/// This namespace contains the solver plugins for various search algorithms.
namespace probfd_plugins::solvers {

extern void add_base_solver_options_to_feature(plugins::Feature& feature);

extern std::tuple<
    utils::Verbosity,
    std::shared_ptr<probfd::TaskCostFunctionFactory>,
    std::vector<std::shared_ptr<::Evaluator>>,
    bool,
    std::shared_ptr<probfd::TaskEvaluatorFactory>,
    std::optional<probfd::value_t>,
    bool,
    double,
    std::string,
    bool>
get_base_solver_args_from_options(const plugins::Options& options);

} // namespace probfd_plugins::solvers

#endif