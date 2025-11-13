#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::merge_and_shrink {

void add_merge_selector_score_based_filtering_feature(
    downward::cli::plugins::Registry& registry);

}

#endif