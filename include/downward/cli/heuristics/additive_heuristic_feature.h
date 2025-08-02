#ifndef DOWNWARD_HEURISTICS_ADDITIVE_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_ADDITIVE_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_additive_heuristic_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif