#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::merge_and_shrink {

language::parser::InternalFunctionDefinitionBase&
add_merge_selector_score_based_filtering_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif