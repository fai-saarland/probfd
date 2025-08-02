#ifndef DOWNWARD_HEURISTICS_FF_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_FF_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_ff_heuristic_features(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif