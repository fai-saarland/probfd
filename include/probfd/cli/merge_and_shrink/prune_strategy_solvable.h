#ifndef PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_SOLVABLE_H
#define PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_SOLVABLE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::merge_and_shrink {

language::parser::InternalFunctionDefinitionBase&
add_prune_strategy_solvable_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif