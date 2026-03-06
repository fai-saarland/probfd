#ifndef DOWNWARD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::heuristics {

void add_additive_cartesian_heuristic_categories(
    language::parser::NamespaceLevelDeclarationList& nspace);

language::parser::InternalFunctionDefinitionBase&
add_additive_cartesian_heuristic_feature(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace downward::cli::heuristics

#endif