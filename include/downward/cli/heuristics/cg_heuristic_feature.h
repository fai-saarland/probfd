#ifndef DOWNWARD_HEURISTICS_CG_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_CG_HEURISTIC_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_cg_heuristic_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif