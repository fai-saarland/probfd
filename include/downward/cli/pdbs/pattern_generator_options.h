#ifndef DOWNWARD_PLUGINS_PDBS_PATTERN_GENERATOR_H
#define DOWNWARD_PLUGINS_PDBS_PATTERN_GENERATOR_H

#include <tuple>

namespace utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::cli::pdbs {

extern void add_generator_options_to_feature(plugins::Feature& feature);

extern std::tuple<utils::Verbosity>
get_generator_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli::pdbs

#endif
