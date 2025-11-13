#ifndef DOWNWARD_HEURISTICS_LANDMARK_CUT_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_CUT_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_landmark_cut_heuristic_feature(
    downward::cli::plugins::Registry& registry);

}

#endif