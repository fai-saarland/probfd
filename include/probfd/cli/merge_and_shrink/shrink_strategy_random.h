#ifndef PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_RANDOM_H
#define PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_RANDOM_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::merge_and_shrink {

language::parser::InternalFunctionDefinitionBase&
add_shrink_strategy_random_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif