#ifndef PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_CATEGORY_H
#define PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif