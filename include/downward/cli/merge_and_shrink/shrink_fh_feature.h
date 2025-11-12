#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_SHRINK_FH_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_SHRINK_FH_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::merge_and_shrink {

void add_shrink_fh_feature(
    downward::cli::plugins::Registry& raw_registry);

}

#endif