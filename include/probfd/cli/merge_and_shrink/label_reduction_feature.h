#ifndef PROBFD_CLI_MERGE_AND_SHRINK_LABEL_REDUCTION_FEATURE_H
#define PROBFD_CLI_MERGE_AND_SHRINK_LABEL_REDUCTION_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::merge_and_shrink {

void add_label_reduction_category(
    language::plugins::Registry& registry);

void add_label_reduction_features(
    language::plugins::Registry& registry);

}

#endif