#ifndef DOWNWARD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H
#define DOWNWARD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H

#include "downward/utils/timer.h"

#include <tuple>

namespace downward::utils {
enum class Verbosity;
class RandomNumberGenerator;
}

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace downward::cli::pdbs {

extern void
add_hillclimbing_options_to_feature(language::plugins::Feature& feature);

std::tuple<
    int,
    int,
    int,
    int,
    utils::FSeconds,
    std::shared_ptr<downward::utils::RandomNumberGenerator>>
get_hillclimbing_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli::pdbs

#endif
