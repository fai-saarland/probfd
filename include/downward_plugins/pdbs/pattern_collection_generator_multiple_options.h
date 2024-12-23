#ifndef DOWNWARD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
#define DOWNWARD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H

#include <tuple>

namespace utils {
enum class Verbosity;
}

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace downward_plugins::pdbs {

extern void add_multiple_algorithm_implementation_notes_to_feature(
    plugins::Feature& feature);

extern void add_multiple_options_to_feature(plugins::Feature& feature);

extern std::
    tuple<int, int, double, double, double, double, bool, int, utils::Verbosity>
    get_multiple_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins::pdbs

#endif
