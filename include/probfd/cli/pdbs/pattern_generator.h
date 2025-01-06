#ifndef PROBFD_PLUGINS_PDBS_PATTERN_GENERATOR_H
#define PROBFD_PLUGINS_PDBS_PATTERN_GENERATOR_H

#include <tuple>

// Forward Declarations
namespace utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace probfd::cli::pdbs {

extern void add_pattern_generator_options_to_feature(
    downward::cli::plugins::Feature& feature);

extern std::tuple<utils::Verbosity> get_generator_arguments_from_options(
    const downward::cli::plugins::Options& opts);

} // namespace probfd::cli::pdbs

#endif
