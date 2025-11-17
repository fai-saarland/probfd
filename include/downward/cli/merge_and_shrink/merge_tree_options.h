#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_TREE_OPTIONS_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_TREE_OPTIONS_H

#include <cstddef>

namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli::merge_and_shrink {

std::size_t add_merge_tree_options_to_feature(
    plugins::Feature& feature,
    std::size_t start_index);

} // namespace downward::cli::merge_and_shrink

#endif