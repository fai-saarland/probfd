#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_OPTIONS_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_OPTIONS_H

#include "language/plugins/options.h"

#include "downward/utils/timer.h"

#include <memory>

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace language {
class Context;
} // namespace language

namespace probfd::merge_and_shrink {
class MergeStrategyFactory;
class ShrinkStrategy;
class LabelReduction;
class PruneStrategy;
} // namespace probfd::merge_and_shrink

namespace probfd::cli::merge_and_shrink {

extern void add_merge_and_shrink_algorithm_options_to_feature(
    language::plugins::Feature& feature);

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
    const language::Context& context,
    const language::plugins::Options& opts);

extern void add_transition_system_size_limit_options_to_feature(
    language::plugins::Feature& feature);

extern std::tuple<int, int, int>
get_transition_system_size_limit_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

extern void handle_shrink_limit_options_defaults(
    int& max_states,
    int& max_states_before_merge,
    int& threshold_before_merge,
    const language::Context& context);

} // namespace probfd::cli::merge_and_shrink

#endif
