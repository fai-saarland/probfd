#ifndef DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H
#define DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H

#include <tuple>

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace downward::cli::utils {

// Add random_seed option to feature.
extern void add_rng_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<int> get_rng_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli::utils

#endif
