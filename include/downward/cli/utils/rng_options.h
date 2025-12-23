#ifndef DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H
#define DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H

#include <cstddef>

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::utils {

extern void add_rng_type(language::plugins::Namespace& nspace);

extern void add_default_rng_function(language::plugins::Namespace& nspace);
extern void add_seeded_rng_function(language::plugins::Namespace& nspace);

// Add random_seed option to feature.
extern std::size_t add_rng_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::utils

#endif
