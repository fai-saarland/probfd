#ifndef DOWNWARD_HEURISTICS_LANDMARK_COST_PARTITIONING_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_COST_PARTITIONING_HEURISTIC_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace downward::cli::heuristics {

void add_landmark_cost_partitioning_heuristic_categories(
    language::plugins::Namespace& nspace);

language::plugins::InternalFunctionDefinitionBase&
add_landmark_cost_partitioning_heuristic_feature(
    language::plugins::Namespace& nspace);

} // namespace downward::cli::heuristics

#endif