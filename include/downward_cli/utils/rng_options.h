#ifndef DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H
#define DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H

#include "language/plugins/raw_registry.h"

#include <memory>
#include <tuple>

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace downward::utils {
class RandomNumberGenerator;
}

namespace downward::cli::utils {

// Add random_seed option to feature.
extern void add_rng_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<std::shared_ptr<downward::utils::RandomNumberGenerator>>
get_rng_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

void add_rng_category(language::plugins::RawRegistry& raw_registry);
void add_rng_features(language::plugins::RawRegistry& raw_registry);

} // namespace downward::cli::utils

#endif
