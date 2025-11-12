#ifndef DOWNWARD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_additive_cartesian_heuristic_categories(
    downward::cli::plugins::Registry& raw_registry);

void add_additive_cartesian_heuristic_feature(
    downward::cli::plugins::Registry& raw_registry);

} // namespace downward::cli::heuristics

#endif