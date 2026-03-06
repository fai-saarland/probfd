#ifndef DOWNWARD_HEURISTICS_IPDBS_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_IPDBS_HEURISTIC_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_ipdbs_heuristic_features(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif