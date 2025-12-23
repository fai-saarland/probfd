#ifndef DOWNWARD_HEURISTICS_SAMPLE_BASED_POTENTIAL_HEURISTICS_FEATURE_H
#define DOWNWARD_HEURISTICS_SAMPLE_BASED_POTENTIAL_HEURISTICS_FEATURE_H

namespace language::plugins {
class Registry;
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::heuristics {

void add_sample_based_potential_heuristics_feature(
    language::plugins::Registry& registry);

language::plugins::InternalFunctionDefinitionBase&
add_sample_based_potential_heuristics_feature(
    language::plugins::Namespace& nspace);

} // namespace downward::cli::heuristics

#endif