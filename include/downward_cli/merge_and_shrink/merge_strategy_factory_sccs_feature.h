#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::merge_and_shrink {

void add_merge_strategy_factory_sccs_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif