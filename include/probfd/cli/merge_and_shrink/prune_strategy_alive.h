#ifndef PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_ALIVE_H
#define PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_ALIVE_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_alive_feature(
    language::plugins::Registry& registry);

}

#endif