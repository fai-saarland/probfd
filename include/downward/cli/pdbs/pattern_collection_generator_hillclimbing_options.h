#ifndef DOWNWARD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H
#define DOWNWARD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H

#include <tuple>

namespace utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::cli::pdbs {

extern void add_hillclimbing_options_to_feature(plugins::Feature& feature);

std::tuple<int, int, int, int, double, int>
get_hillclimbing_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli::pdbs

#endif
