#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_TREE_OPTIONS_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_TREE_OPTIONS_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace downward::cli::merge_and_shrink {

std::size_t add_merge_tree_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::merge_and_shrink

#endif