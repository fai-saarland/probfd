#ifndef CLI_POTENTIALS_POTENTIAL_OPTIONS_H
#define CLI_POTENTIALS_POTENTIAL_OPTIONS_H

#include "downward/lp/lp_solver.h"

#include <memory>
#include <string>

namespace downward {
class TaskTransformation;

enum class LPSolverType;
} // namespace downward

namespace downward::utils {
enum class Verbosity;
}

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace downward::cli::potentials {

std::string get_admissible_potentials_reference();

void add_admissible_potentials_options_to_feature(
    language::plugins::Feature& feature,
    const std::string& description);

std::tuple<
    double,
    lp::LPSolverType,
    std::shared_ptr<TaskTransformation>,
    bool,
    std::string,
    utils::Verbosity>
get_admissible_potential_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli::potentials

#endif // CLI_POTENTIALS_POTENTIAL_OPTIONS_H
