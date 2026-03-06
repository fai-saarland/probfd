#ifndef PROBFD_CLI_HEURISTICS_DETERMINIZATION_COST_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_DETERMINIZATION_COST_HEURISTIC_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_determinization_cost_heuristic_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif