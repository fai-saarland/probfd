#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_merge_selector_score_based_filtering_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif