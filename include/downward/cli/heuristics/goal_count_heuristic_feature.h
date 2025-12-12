#ifndef DOWNWARD_HEURISTICS_GOAL_COUNT_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_GOAL_COUNT_HEURISTIC_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_goal_count_heuristic_features(
    language::plugins::Registry& registry);

}

#endif