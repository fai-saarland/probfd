#ifndef PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_RANDOM_H
#define PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_RANDOM_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_random_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif