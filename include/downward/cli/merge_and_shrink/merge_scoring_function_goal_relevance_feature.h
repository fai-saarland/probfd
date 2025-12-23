#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_GOAL_RELEVANCE_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_GOAL_RELEVANCE_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::merge_and_shrink {

language::plugins::InternalFunctionDefinitionBase&
add_merge_scoring_function_goal_relevance_feature(
    language::plugins::Namespace& nspace);

}

#endif