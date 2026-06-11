#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::merge_and_shrink {

void add_merge_selector_score_based_filtering_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif