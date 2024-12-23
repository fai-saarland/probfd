#ifndef DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H
#define DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H

#include <tuple>

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace downward_plugins::utils {

// Add random_seed option to feature.
extern void add_rng_options_to_feature(plugins::Feature& feature);

extern std::tuple<int>
get_rng_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins::utils

#endif
