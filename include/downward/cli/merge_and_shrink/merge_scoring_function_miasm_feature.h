#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_miasm_feature(
    language::plugins::Registry& registry);

}

#endif