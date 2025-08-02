#ifndef DOWNWARD_HEURISTICS_LANDMARK_CUT_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_CUT_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_landmark_cut_heuristic_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif