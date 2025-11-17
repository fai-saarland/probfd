#ifndef DOWNWARD_PLUGINS_PRUNING_METHOD_H
#define DOWNWARD_PLUGINS_PRUNING_METHOD_H

#include <tuple>

namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli {

extern std::size_t add_pruning_options_to_feature(
    plugins::Feature& feature,
    std::size_t start_index);

} // namespace downward::cli

#endif
