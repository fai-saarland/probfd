#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_TOTAL_ORDER_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_TOTAL_ORDER_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_total_order_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif