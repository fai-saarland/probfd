#ifndef DOWNWARD_HEURISTICS_LANDMARK_COST_PARTITIONING_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_COST_PARTITIONING_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_landmark_cost_partitioning_heuristic_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif