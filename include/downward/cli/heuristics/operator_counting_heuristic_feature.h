#ifndef DOWNWARD_HEURISTICS_OPERATOR_COUNTING_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_OPERATOR_COUNTING_HEURISTIC_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_operator_counting_heuristic_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif