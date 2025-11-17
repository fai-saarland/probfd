#ifndef DOWNWARD_PLUGINS_PDBS_PATTERN_GENERATOR_H
#define DOWNWARD_PLUGINS_PDBS_PATTERN_GENERATOR_H

#include <cstddef>

namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli::pdbs {

extern std::size_t add_generator_options_to_feature(
    plugins::Feature& feature,
    std::size_t start_index);

} // namespace downward::cli::pdbs

#endif
