#ifndef DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H
#define DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H

#include <cstddef>

namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli::utils {

// Add random_seed option to feature.
extern std::size_t
add_rng_options_to_feature(plugins::Feature& feature, std::size_t start_index);

} // namespace downward::cli::utils

#endif
