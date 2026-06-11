#ifndef DOWNWARD_HEURISTICS_CANONICAL_PDBS_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_CANONICAL_PDBS_HEURISTIC_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_canonical_pdbs_heuristic_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif