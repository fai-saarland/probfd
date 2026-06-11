#ifndef DOWNWARD_HEURISTICS_ZERO_ONE_PDB_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_ZERO_ONE_PDB_HEURISTIC_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_zero_one_pdbs_heuristic_features(
    language::plugins::RawRegistry& raw_registry);

}

#endif