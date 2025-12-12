#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_TREE_OPTIONS_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_TREE_OPTIONS_H

#include <cstddef>

namespace language::plugins {
class InternalFunctionDefinitionBase;
} // namespace language::plugins

namespace downward::cli::merge_and_shrink {

std::size_t add_merge_tree_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::merge_and_shrink

#endif