#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_H

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

namespace downward::cli::merge_and_shrink {

extern void
add_merge_strategy_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<utils::Verbosity> get_merge_strategy_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli::merge_and_shrink

#endif
