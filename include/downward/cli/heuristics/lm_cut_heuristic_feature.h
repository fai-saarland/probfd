#ifndef DOWNWARD_HEURISTICS_LANDMARK_CUT_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_CUT_HEURISTIC_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_landmark_cut_heuristic_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif