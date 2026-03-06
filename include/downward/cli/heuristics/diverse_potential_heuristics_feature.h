#ifndef DOWNWARD_HEURISTICS_DIVERSE_POTENTIAL_HEURISTICS_FEATURE_H
#define DOWNWARD_HEURISTICS_DIVERSE_POTENTIAL_HEURISTICS_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_diverse_potential_heuristics_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif