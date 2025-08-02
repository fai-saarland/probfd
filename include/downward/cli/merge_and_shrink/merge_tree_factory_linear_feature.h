#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::merge_and_shrink {

void add_merge_tree_factory_linear_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif