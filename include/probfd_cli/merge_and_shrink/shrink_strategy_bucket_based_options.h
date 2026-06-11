
#ifndef PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_BUCKET_BASED_OPTIONS_H
#define PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_BUCKET_BASED_OPTIONS_H

#include "downward/utils/logging.h"

#include <tuple>

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace probfd::cli::merge_and_shrink {

extern void
add_bucket_based_shrink_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<int> get_bucket_based_shrink_args_from_options(
    const language::Context& context,
    const language::plugins::Options& options);

} // namespace probfd::cli::merge_and_shrink

#endif
