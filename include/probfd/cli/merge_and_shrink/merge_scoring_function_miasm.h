#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace probfd::cli::merge_and_shrink {

language::plugins::InternalFunctionDefinitionBase&
add_merge_scoring_function_miasm_feature(language::plugins::Namespace& nspace);

} // namespace probfd::cli::merge_and_shrink

#endif