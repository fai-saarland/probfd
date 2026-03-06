#ifndef PROBFD_CLI_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_pdb_heuristic_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif