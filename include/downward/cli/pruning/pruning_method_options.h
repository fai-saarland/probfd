#ifndef DOWNWARD_PLUGINS_PRUNING_METHOD_H
#define DOWNWARD_PLUGINS_PRUNING_METHOD_H

#include <tuple>
#include <vector>

namespace downward::utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Options;
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli {

extern void add_pruning_options_to_feature(plugins::Feature& feature);

extern std::tuple<utils::Verbosity>
get_pruning_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli

#endif
