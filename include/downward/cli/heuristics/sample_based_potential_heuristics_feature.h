#ifndef DOWNWARD_HEURISTICS_SAMPLE_BASED_POTENTIAL_HEURISTICS_FEATURE_H
#define DOWNWARD_HEURISTICS_SAMPLE_BASED_POTENTIAL_HEURISTICS_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::heuristics {

void add_sample_based_potential_heuristics_feature(
    language::plugins::Registry& registry);

}

#endif