#ifndef PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_PROBABILISTIC_BISIMULATION_H
#define PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_PROBABILISTIC_BISIMULATION_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_probabilistic_bisimulation_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif