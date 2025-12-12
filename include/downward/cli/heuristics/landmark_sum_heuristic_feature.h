#ifndef DOWNWARD_HEURISTICS_LANDMARK_SUM_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_SUM_HEURISTIC_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_landmark_sum_heuristic_feature(
    language::plugins::Registry& registry);

}

#endif