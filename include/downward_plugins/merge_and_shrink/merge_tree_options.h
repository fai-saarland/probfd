#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_TREE_OPTIONS_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_TREE_OPTIONS_H

#include <tuple>

namespace merge_and_shrink {
enum class UpdateOption;
}

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace downward_plugins::merge_and_shrink {

void add_merge_tree_options_to_feature(plugins::Feature& feature);

std::tuple<int, ::merge_and_shrink::UpdateOption>
get_merge_tree_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins::merge_and_shrink

#endif