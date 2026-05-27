#ifndef DOWNWARD_HEURISTICS_MAX_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_MAX_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_max_heuristic_feature(
    downward::cli::plugins::Registry& registry);

}

#endif