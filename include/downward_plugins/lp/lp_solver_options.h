#ifndef DOWNWARD_PLUGINS_LP_LP_SOLVER_H
#define DOWNWARD_PLUGINS_LP_LP_SOLVER_H

#include <tuple>

namespace lp {
enum class LPSolverType;
}

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace downward_plugins::lp {

void add_lp_solver_option_to_feature(plugins::Feature& feature);

std::tuple<::lp::LPSolverType>
get_lp_solver_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins::lp

#endif