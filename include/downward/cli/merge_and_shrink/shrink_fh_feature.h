#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_SHRINK_FH_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_SHRINK_FH_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::merge_and_shrink {

void add_shrink_fh_feature(
    language::plugins::Registry& registry);

}

#endif