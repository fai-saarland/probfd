#ifndef DOWNWARD_HEURISTICS_DIVERSE_POTENTIAL_HEURISTICS_FEATURE_H
#define DOWNWARD_HEURISTICS_DIVERSE_POTENTIAL_HEURISTICS_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_diverse_potential_heuristics_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif