#ifndef DOWNWARD_HEURISTICS_PDB_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_PDB_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_pdb_heuristic_feature(
    downward::cli::plugins::Registry& registry);

}

#endif