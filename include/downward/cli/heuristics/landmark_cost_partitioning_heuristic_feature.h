#ifndef DOWNWARD_HEURISTICS_LANDMARK_COST_PARTITIONING_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_COST_PARTITIONING_HEURISTIC_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::heuristics {

void add_landmark_cost_partitioning_heuristic_categories(
    language::parser::NamespaceLevelDeclarationList& nspace);

language::parser::InternalFunctionDefinitionBase&
add_landmark_cost_partitioning_heuristic_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace downward::cli::heuristics

#endif