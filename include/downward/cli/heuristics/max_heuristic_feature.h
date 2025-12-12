#ifndef DOWNWARD_HEURISTICS_MAX_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_MAX_HEURISTIC_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_max_heuristic_feature(
    language::plugins::Registry& registry);

}

#endif