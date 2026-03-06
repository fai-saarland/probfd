#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_LABEL_REDUCTION_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_LABEL_REDUCTION_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::merge_and_shrink {

void add_label_reduction_category(language::parser::NamespaceLevelDeclarationList& nspace);

language::parser::InternalFunctionDefinitionBase&
add_label_reduction_to_namespace(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace downward::cli::merge_and_shrink

#endif