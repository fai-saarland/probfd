#ifndef DOWNWARD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::heuristics {

void add_additive_cartesian_heuristic_categories(
    language::plugins::Namespace& nspace);

language::plugins::InternalFunctionDefinitionBase&
add_additive_cartesian_heuristic_feature(language::plugins::Namespace& nspace);

} // namespace downward::cli::heuristics

#endif