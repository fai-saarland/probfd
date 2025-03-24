#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_OPTIONS_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_OPTIONS_H

#include <memory>

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace utils {
class Context;
} // namespace downward::cli::plugins

namespace probfd::cli::merge_and_shrink {

extern void add_merge_and_shrink_algorithm_options_to_feature(
    downward::cli::plugins::Feature& feature);
extern void add_transition_system_size_limit_options_to_feature(
    downward::cli::plugins::Feature& feature);

extern void handle_shrink_limit_options_defaults(
    downward::cli::plugins::Options& opts,
    const utils::Context& context);

} // namespace probfd::merge_and_shrink

#endif
