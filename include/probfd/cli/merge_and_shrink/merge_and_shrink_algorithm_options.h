#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_OPTIONS_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_OPTIONS_H

#include <cstddef>

namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace downward::utils {
class Context;
} // namespace downward::utils

namespace probfd::cli::merge_and_shrink {

extern std::size_t add_transition_system_size_limit_options_to_feature(
    downward::cli::plugins::Feature& feature,
    std::size_t start_index);

extern void handle_shrink_limit_options_defaults(
    int& max_states,
    int& max_states_before_merge,
    int& threshold_before_merge,
    const downward::utils::Context& context);

} // namespace probfd::cli::merge_and_shrink

#endif
