#ifndef DOWNWARD_HEURISTICS_LANDMARK_COST_PARTITIONING_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_COST_PARTITIONING_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_landmark_cost_partitioning_heuristic_categories(
    downward::cli::plugins::Registry& registry);

void add_landmark_cost_partitioning_heuristic_feature(
    downward::cli::plugins::Registry& registry);

} // namespace downward::cli::heuristics

#endif