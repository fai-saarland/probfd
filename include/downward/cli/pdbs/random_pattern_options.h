#ifndef DOWNWARD_PLUGINS_PDBS_RANDOM_PATTERN_H
#define DOWNWARD_PLUGINS_PDBS_RANDOM_PATTERN_H

#include <cstddef>

namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli::pdbs {

extern void
add_random_pattern_implementation_notes_to_feature(plugins::Feature& feature);

extern std::size_t add_random_pattern_bidirectional_option_to_feature(
    plugins::Feature& feature,
    std::size_t start_index);

} // namespace downward::cli::pdbs

#endif
