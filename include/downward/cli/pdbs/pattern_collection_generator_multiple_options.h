#ifndef DOWNWARD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
#define DOWNWARD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H

#include <cstddef>

namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli::pdbs {

extern void add_multiple_algorithm_implementation_notes_to_feature(
    plugins::Feature& feature);

extern std::size_t add_multiple_options_to_feature(
    plugins::Feature& feature,
    std::size_t start_index);

} // namespace downward::cli::pdbs

#endif
