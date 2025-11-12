#ifndef PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_SOLVABLE_H
#define PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_SOLVABLE_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_solvable_feature(
    downward::cli::plugins::Registry& raw_registry);

}

#endif