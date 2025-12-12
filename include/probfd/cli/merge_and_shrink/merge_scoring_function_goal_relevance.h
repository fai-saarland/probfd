#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_GOAL_RELEVANCE_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_GOAL_RELEVANCE_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::merge_and_shrink {

void add_merge_scoring_function_goal_relevance_feature(
    language::plugins::Registry& registry);

}

#endif