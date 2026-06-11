#ifndef PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_RANDOM_H
#define PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_RANDOM_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_random_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif