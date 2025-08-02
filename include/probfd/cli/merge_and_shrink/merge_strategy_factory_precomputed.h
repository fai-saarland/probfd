#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_PRECOMPUTED_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_PRECOMPUTED_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_precomputed_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif