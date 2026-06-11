#ifndef DOWNWARD_HEURISTICS_SINGLE_POTENTIAL_HEURISTICS_FEATURE_H
#define DOWNWARD_HEURISTICS_SINGLE_POTENTIAL_HEURISTICS_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_single_potential_heuristics_features(
    language::plugins::RawRegistry& raw_registry);

}

#endif