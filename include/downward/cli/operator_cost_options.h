#ifndef DOWNWARD_PLUGINS_OPERATOR_COST_H
#define DOWNWARD_PLUGINS_OPERATOR_COST_H

#include <cstddef>

namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli {

extern std::size_t add_cost_type_options_to_feature(
    plugins::Feature& feature,
    std::size_t start_index);

} // namespace downward::cli

#endif
