#ifndef DOWNWARD_HEURISTICS_MAX_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_MAX_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_max_heuristic_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif