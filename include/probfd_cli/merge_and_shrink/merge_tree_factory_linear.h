#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_merge_tree_factory_linear_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif