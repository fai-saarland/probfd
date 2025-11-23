#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_OPTIONS_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_OPTIONS_H

#include <cstddef>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
} // namespace downward::cli::plugins

namespace probfd::cli::merge_and_shrink {

extern std::size_t add_transition_system_size_limit_options_to_feature(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

extern void handle_shrink_limit_options_defaults(
    int& max_states,
    int& max_states_before_merge,
    int& threshold_before_merge);

} // namespace probfd::cli::merge_and_shrink

#endif
