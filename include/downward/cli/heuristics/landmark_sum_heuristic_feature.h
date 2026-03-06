#ifndef DOWNWARD_HEURISTICS_LANDMARK_SUM_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_SUM_HEURISTIC_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_landmark_sum_heuristic_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif