#ifndef PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_IDENTITY_H
#define PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_IDENTITY_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_identity_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif