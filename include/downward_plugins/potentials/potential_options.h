#ifndef DOWNWARD_PLUGINS_POTENTIALS_UTIL_H
#define DOWNWARD_PLUGINS_POTENTIALS_UTIL_H

#include "downward/lp/lp_solver.h"

#include <memory>
#include <string>

class AbstractTask;

enum class LPSolverType;

namespace utils {
enum class Verbosity;
}

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace downward_plugins::potentials {

std::string get_admissible_potentials_reference();

void add_admissible_potentials_options_to_feature(
    plugins::Feature& feature,
    const std::string& description);

std::tuple<
    double,
    lp::LPSolverType,
    std::shared_ptr<AbstractTask>,
    bool,
    std::string,
    utils::Verbosity>
get_admissible_potential_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins::potentials

#endif
