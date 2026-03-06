#ifndef DOWNWARD_CLI_PRUNING_LIMITED_PRUNING_FEATURE_H
#define DOWNWARD_CLI_PRUNING_LIMITED_PRUNING_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::pruning {

language::parser::InternalFunctionDefinitionBase&
add_limited_pruning_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif