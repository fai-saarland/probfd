#ifndef PROBFD_CLI_HEURISTICS_CONSTANT_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_CONSTANT_HEURISTIC_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_blind_heuristic_factory_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif