#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_STATELESS_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_STATELESS_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::merge_and_shrink {

language::parser::InternalFunctionDefinitionBase&
add_merge_strategy_factory_stateless_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif