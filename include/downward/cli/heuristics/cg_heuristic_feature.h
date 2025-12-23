#ifndef DOWNWARD_HEURISTICS_CG_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_CG_HEURISTIC_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace downward::cli::heuristics {

language::plugins::InternalFunctionDefinitionBase&
add_cg_heuristic_feature(language::plugins::Namespace& nspace);

}

#endif