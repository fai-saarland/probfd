#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_SHRINK_RANDOM_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_SHRINK_RANDOM_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::merge_and_shrink {

void add_shrink_random_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif