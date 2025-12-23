#ifndef PROBFD_CLI_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace probfd::cli::heuristics {

language::plugins::InternalFunctionDefinitionBase&
add_pdb_heuristic_feature(language::plugins::Namespace& nspace);

}

#endif