#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_SHRINK_FH_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_SHRINK_FH_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::merge_and_shrink {

void add_shrink_fh_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif