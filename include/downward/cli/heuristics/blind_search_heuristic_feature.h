#ifndef DOWNWARD_HEURISTICS_BLIND_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_BLIND_HEURISTIC_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_blind_heuristic_feature(
    downward::cli::plugins::Registry& registry);

}

#endif