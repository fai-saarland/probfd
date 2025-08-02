#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_SINGLE_RANDOM_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_SINGLE_RANDOM_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_merge_scoring_function_single_random_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif