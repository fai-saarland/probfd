#ifndef DOWNWARD_HEURISTICS_ZERO_ONE_PDB_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_ZERO_ONE_PDB_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_zero_one_pdbs_heuristic_features(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif