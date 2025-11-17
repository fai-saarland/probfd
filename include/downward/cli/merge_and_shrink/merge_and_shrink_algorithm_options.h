#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H

#include <cstddef>

namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace downward::utils {
class Context;
}

namespace downward::cli::merge_and_shrink {

extern std::size_t add_transition_system_size_limit_options_to_feature(
    plugins::Feature& feature,
    std::size_t start_index);

extern void handle_shrink_limit_options_defaults(
    int& max_states,
    int& max_states_before_merge,
    int& threshold_before_merge,
    const utils::Context& context);

} // namespace downward::cli::merge_and_shrink

#endif
