#ifndef DOWNWARD_HEURISTICS_CG_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_CG_HEURISTIC_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_cg_heuristic_feature(
    language::plugins::Registry& registry);

}

#endif