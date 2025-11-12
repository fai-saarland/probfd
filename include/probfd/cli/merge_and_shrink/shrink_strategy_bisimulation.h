#ifndef PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_BISIMULATION_H
#define PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_BISIMULATION_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_bisimulation_feature(
    downward::cli::plugins::Registry& raw_registry);

}

#endif