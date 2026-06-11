#ifndef DOWNWARD_PLUGINS_PDBS_PATTERN_GENERATOR_H
#define DOWNWARD_PLUGINS_PDBS_PATTERN_GENERATOR_H

#include <tuple>

namespace downward::utils {
enum class Verbosity;
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
add_generator_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<downward::utils::Verbosity>
get_generator_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli::pdbs

#endif
