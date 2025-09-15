#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_DFP_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_DFP_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_dfp_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif