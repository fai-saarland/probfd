#ifndef DOWNWARD_HEURISTICS_MERGE_AND_SHRINK_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_MERGE_AND_SHRINK_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_merge_and_shrink_heuristic_feature(
    downward::cli::plugins::Registry& registry);

}

#endif