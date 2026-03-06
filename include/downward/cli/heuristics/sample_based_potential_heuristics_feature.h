#ifndef DOWNWARD_HEURISTICS_SAMPLE_BASED_POTENTIAL_HEURISTICS_FEATURE_H
#define DOWNWARD_HEURISTICS_SAMPLE_BASED_POTENTIAL_HEURISTICS_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_sample_based_potential_heuristics_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace downward::cli::heuristics

#endif