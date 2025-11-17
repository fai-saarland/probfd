#ifndef DOWNWARD_PLUGINS_LP_LP_SOLVER_H
#define DOWNWARD_PLUGINS_LP_LP_SOLVER_H

#include <cstddef>

namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli::lp {

std::size_t add_lp_solver_option_to_feature(
    plugins::Feature& feature,
    std::size_t start_index);

} // namespace downward::cli::lp

#endif