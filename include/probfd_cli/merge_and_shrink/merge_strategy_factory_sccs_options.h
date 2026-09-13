#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_OPTIONS_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_OPTIONS_H

#include "downward/utils/logging.h"

#include <tuple>

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace probfd::merge_and_shrink {
enum class OrderOfSCCs;
}

namespace probfd::cli::merge_and_shrink {

extern void
add_merge_strategy_sccs_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<probfd::merge_and_shrink::OrderOfSCCs>
get_merge_strategy_sccs_args_from_options(
    const language::Context& context,
    const language::plugins::Options& options);

} // namespace probfd::cli::merge_and_shrink

#endif