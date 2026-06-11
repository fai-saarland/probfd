#ifndef DOWNWARD_HEURISTICS_OPERATOR_COUNTING_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_OPERATOR_COUNTING_HEURISTIC_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::heuristics {

void add_operator_counting_heuristic_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif