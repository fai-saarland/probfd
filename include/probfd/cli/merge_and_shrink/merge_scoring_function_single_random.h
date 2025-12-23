#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_SINGLE_RANDOM_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_SINGLE_RANDOM_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace probfd::cli::merge_and_shrink {

language::plugins::InternalFunctionDefinitionBase&
add_merge_scoring_function_single_random_feature(
    language::plugins::Namespace& nspace);

}

#endif