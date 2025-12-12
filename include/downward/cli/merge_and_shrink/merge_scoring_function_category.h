#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_CATEGORY_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_CATEGORY_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_category(
    language::plugins::Registry& registry);

}

#endif