#ifndef DOWNWARD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_additive_cartesian_heuristic_categories(
    language::plugins::Registry& registry);

void add_additive_cartesian_heuristic_feature(
    language::plugins::Registry& registry);

} // namespace downward::cli::heuristics

#endif