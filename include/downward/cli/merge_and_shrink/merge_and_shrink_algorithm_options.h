#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H

#include <memory>
#include <tuple>

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace merge_and_shrink {
class LabelReduction;
class MergeStrategyFactory;
class ShrinkStrategy;
} // namespace merge_and_shrink

namespace utils {
class Context;
}

namespace downward::cli::merge_and_shrink {

extern void
add_merge_and_shrink_algorithm_options_to_feature(plugins::Feature& feature);
std::tuple<
    std::shared_ptr<::merge_and_shrink::MergeStrategyFactory>,
    std::shared_ptr<::merge_and_shrink::ShrinkStrategy>,
    std::shared_ptr<::merge_and_shrink::LabelReduction>,
    bool,
    bool,
    int,
    int,
    int,
    double>
get_merge_and_shrink_algorithm_arguments_from_options(
    const plugins::Options& opts);

extern void
add_transition_system_size_limit_options_to_feature(plugins::Feature& feature);

std::tuple<int, int, int>
get_transition_system_size_limit_arguments_from_options(
    const plugins::Options& opts);

extern void handle_shrink_limit_options_defaults(
    plugins::Options& opts,
    const utils::Context& context);

} // namespace downward::cli::merge_and_shrink

#endif
