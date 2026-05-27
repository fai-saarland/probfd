#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_LABEL_REDUCTION_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_LABEL_REDUCTION_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::merge_and_shrink {

void add_label_reduction_category(
    downward::cli::plugins::Registry& registry);

void add_label_reduction_features(
    downward::cli::plugins::Registry& registry);

}

#endif