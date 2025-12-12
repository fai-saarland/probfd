#ifndef DOWNWARD_HEURISTICS_IPDBS_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_IPDBS_HEURISTIC_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_ipdbs_heuristic_features(
    language::plugins::Registry& registry);

}

#endif