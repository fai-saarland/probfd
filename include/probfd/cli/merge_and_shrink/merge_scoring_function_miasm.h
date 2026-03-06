#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::merge_and_shrink {

language::parser::InternalFunctionDefinitionBase&
add_merge_scoring_function_miasm_feature(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace probfd::cli::merge_and_shrink

#endif