#ifndef PROBFD_CLI_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::heuristics {

void add_pdb_heuristic_feature(
    language::plugins::Registry& registry);

}

#endif