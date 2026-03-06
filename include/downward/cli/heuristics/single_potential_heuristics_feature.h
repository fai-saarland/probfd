#ifndef DOWNWARD_HEURISTICS_SINGLE_POTENTIAL_HEURISTICS_FEATURE_H
#define DOWNWARD_HEURISTICS_SINGLE_POTENTIAL_HEURISTICS_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_initial_state_potential_heuristic_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

language::parser::InternalFunctionDefinitionBase&
add_all_states_potential_heuristic_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace downward::cli::heuristics

#endif