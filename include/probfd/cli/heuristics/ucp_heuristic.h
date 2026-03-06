#ifndef PROBFD_CLI_HEURISTICS_UCP_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_UCP_HEURISTIC_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_ucp_heuristic_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif