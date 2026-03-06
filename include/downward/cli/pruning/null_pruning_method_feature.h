#ifndef DOWNWARD_CLI_PRUNING_NULL_PRUNING_METHOD_FEATURE_H
#define DOWNWARD_CLI_PRUNING_NULL_PRUNING_METHOD_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::pruning {

language::parser::InternalFunctionDefinitionBase&
add_null_pruning_method_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif