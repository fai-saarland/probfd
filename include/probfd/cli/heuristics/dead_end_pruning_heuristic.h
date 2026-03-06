#ifndef PROBFD_CLI_HEURISTICS_DEAD_END_PRUNING_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_DEAD_END_PRUNING_HEURISTIC_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_dead_end_pruning_heuristic_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif