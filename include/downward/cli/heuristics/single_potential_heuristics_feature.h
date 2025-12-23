#ifndef DOWNWARD_HEURISTICS_SINGLE_POTENTIAL_HEURISTICS_FEATURE_H
#define DOWNWARD_HEURISTICS_SINGLE_POTENTIAL_HEURISTICS_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::heuristics {

language::plugins::InternalFunctionDefinitionBase&
add_initial_state_potential_heuristic_feature(
    language::plugins::Namespace& nspace);

language::plugins::InternalFunctionDefinitionBase&
add_all_states_potential_heuristic_feature(
    language::plugins::Namespace& nspace);

} // namespace downward::cli::heuristics

#endif