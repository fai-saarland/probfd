#ifndef DOWNWARD_HEURISTICS_HM_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_HM_HEURISTIC_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_hm_heuristic_features(
    language::plugins::Registry& registry);

}

#endif