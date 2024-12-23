#ifndef DOWNWARD_PLUGINS_PRUNING_METHOD_H
#define DOWNWARD_PLUGINS_PRUNING_METHOD_H

#include <tuple>
#include <vector>

namespace utils {
enum class Verbosity;
}

namespace downward_plugins::plugins {
class Options;
class Feature;
} // namespace downward_plugins::plugins

namespace downward_plugins {

extern void add_pruning_options_to_feature(plugins::Feature& feature);

extern std::tuple<utils::Verbosity>
get_pruning_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins

#endif
