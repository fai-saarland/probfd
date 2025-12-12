#ifndef DOWNWARD_HEURISTICS_LANDMARK_COST_PARTITIONING_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_COST_PARTITIONING_HEURISTIC_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_landmark_cost_partitioning_heuristic_categories(
    language::plugins::Registry& registry);

void add_landmark_cost_partitioning_heuristic_feature(
    language::plugins::Registry& registry);

} // namespace downward::cli::heuristics

#endif