#ifndef DOWNWARD_HEURISTICS_FF_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_FF_HEURISTIC_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_ff_heuristic_features(
    language::plugins::RawRegistry& raw_registry);

}

#endif