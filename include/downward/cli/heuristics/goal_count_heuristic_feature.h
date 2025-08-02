#ifndef DOWNWARD_HEURISTICS_GOAL_COUNT_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_GOAL_COUNT_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_goal_count_heuristic_features(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif