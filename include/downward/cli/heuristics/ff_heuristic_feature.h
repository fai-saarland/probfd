#ifndef DOWNWARD_HEURISTICS_FF_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_FF_HEURISTIC_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_ff_heuristic_features(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif