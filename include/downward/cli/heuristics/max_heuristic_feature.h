#ifndef DOWNWARD_HEURISTICS_MAX_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_MAX_HEURISTIC_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_max_heuristic_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif