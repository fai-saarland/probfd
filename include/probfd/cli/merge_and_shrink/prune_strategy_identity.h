#ifndef PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_IDENTITY_H
#define PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_IDENTITY_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::merge_and_shrink {

language::parser::InternalFunctionDefinitionBase&
add_prune_strategy_identity_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif