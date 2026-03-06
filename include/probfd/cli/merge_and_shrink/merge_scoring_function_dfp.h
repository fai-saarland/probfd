#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_DFP_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_DFP_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::merge_and_shrink {

language::parser::InternalFunctionDefinitionBase&
add_merge_scoring_function_dfp_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif