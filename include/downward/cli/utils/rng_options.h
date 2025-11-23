#ifndef DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H
#define DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H

#include <cstddef>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
class Registry;
} // namespace downward::cli::plugins

namespace downward::cli::utils {

extern void add_rng_type(plugins::Registry& registry);

extern void add_default_rng_function(plugins::Registry& registry);
extern void add_seeded_rng_function(plugins::Registry& registry);

// Add random_seed option to feature.
extern std::size_t
add_rng_options_to_feature(plugins::InternalFunctionDefinitionBase& feature, std::size_t start_index);

} // namespace downward::cli::utils

#endif
