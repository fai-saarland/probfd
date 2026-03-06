#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace downward::cli::merge_and_shrink {

extern std::size_t add_transition_system_size_limit_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

extern void handle_shrink_limit_options_defaults(
    int& max_states,
    int& max_states_before_merge,
    int& threshold_before_merge);

} // namespace downward::cli::merge_and_shrink

#endif
