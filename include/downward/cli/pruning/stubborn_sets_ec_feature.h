#ifndef DOWNWARD_CLI_PRUNING_STUBBORN_SETS_EC_FEATURE_H
#define DOWNWARD_CLI_PRUNING_STUBBORN_SETS_EC_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::pruning {

language::parser::InternalFunctionDefinitionBase&
add_stubborn_sets_ec_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif