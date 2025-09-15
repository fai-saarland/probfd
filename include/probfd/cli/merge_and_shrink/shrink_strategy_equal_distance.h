#ifndef PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_EQUAL_DISTANCE_H
#define PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_EQUAL_DISTANCE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_equal_distance_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif