#ifndef DOWNWARD_HEURISTICS_LANDMARK_SUM_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_SUM_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_landmark_sum_heuristic_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif