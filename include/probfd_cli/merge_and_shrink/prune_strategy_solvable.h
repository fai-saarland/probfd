#ifndef PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_SOLVABLE_H
#define PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_SOLVABLE_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_solvable_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif