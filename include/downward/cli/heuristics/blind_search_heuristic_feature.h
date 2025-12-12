#ifndef DOWNWARD_HEURISTICS_BLIND_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_BLIND_HEURISTIC_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_blind_heuristic_feature(
    language::plugins::Registry& registry);

}

#endif