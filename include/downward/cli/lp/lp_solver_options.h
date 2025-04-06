#ifndef DOWNWARD_PLUGINS_LP_LP_SOLVER_H
#define DOWNWARD_PLUGINS_LP_LP_SOLVER_H

#include <tuple>

namespace downward::lp {
enum class LPSolverType;
}

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::cli::lp {

void add_lp_solver_option_to_feature(plugins::Feature& feature);

std::tuple<downward::lp::LPSolverType>
get_lp_solver_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli::lp

#endif