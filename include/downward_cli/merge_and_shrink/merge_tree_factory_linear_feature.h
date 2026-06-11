#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::merge_and_shrink {

void add_merge_tree_factory_linear_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif