#ifndef PROBFD_PLUGINS_PDBS_PATTERN_GENERATOR_H
#define PROBFD_PLUGINS_PDBS_PATTERN_GENERATOR_H

#include <tuple>

// Forward Declarations
namespace utils {
enum class Verbosity;
}

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace probfd_plugins::pdbs {

extern void add_pattern_generator_options_to_feature(
    downward_plugins::plugins::Feature& feature);

extern std::tuple<utils::Verbosity> get_generator_arguments_from_options(
    const downward_plugins::plugins::Options& opts);

} // namespace probfd_plugins::pdbs

#endif
