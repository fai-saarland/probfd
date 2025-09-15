#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_OPTIONS_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_OPTIONS_H

#include "downward/cli/plugins/options.h"

#include <memory>

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::utils {
class Context;
} // namespace utils

namespace probfd::merge_and_shrink {
class MergeStrategyFactory;
class ShrinkStrategy;
class LabelReduction;
class PruneStrategy;
} // namespace probfd::merge_and_shrink

namespace probfd::cli::merge_and_shrink {

extern void add_merge_and_shrink_algorithm_options_to_feature(
    downward::cli::plugins::Feature& feature);

std::tuple<
    std::shared_ptr<probfd::merge_and_shrink::MergeStrategyFactory>,
    std::shared_ptr<probfd::merge_and_shrink::ShrinkStrategy>,
    std::shared_ptr<probfd::merge_and_shrink::LabelReduction>,
    std::shared_ptr<probfd::merge_and_shrink::PruneStrategy>,
    int,
    int,
    int,
    downward::utils::Duration>
get_merge_and_shrink_algorithm_arguments_from_options(
    const downward::cli::plugins::Options& opts);

extern void add_transition_system_size_limit_options_to_feature(
    downward::cli::plugins::Feature& feature);

extern std::tuple<int, int, int>
get_transition_system_size_limit_arguments_from_options(
    const downward::cli::plugins::Options& opts);

extern void handle_shrink_limit_options_defaults(
    downward::cli::plugins::Options& opts,
    const downward::utils::Context& context);

} // namespace probfd::cli::merge_and_shrink

#endif
