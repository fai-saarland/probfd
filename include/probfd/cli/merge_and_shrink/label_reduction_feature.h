#ifndef PROBFD_CLI_MERGE_AND_SHRINK_LABEL_REDUCTION_FEATURE_H
#define PROBFD_CLI_MERGE_AND_SHRINK_LABEL_REDUCTION_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_label_reduction_features(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif