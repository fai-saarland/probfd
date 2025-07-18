#ifndef DOWNWARD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
#define DOWNWARD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H

#include <tuple>

namespace downward::utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::cli::pdbs {

extern void add_multiple_algorithm_implementation_notes_to_feature(
    plugins::Feature& feature);

extern void add_multiple_options_to_feature(plugins::Feature& feature);

extern std::tuple<
    int,
    int,
    utils::Duration,
    utils::Duration,
    utils::Duration,
    double,
    bool,
    int,
    utils::Verbosity>
get_multiple_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli::pdbs

#endif
