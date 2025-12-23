#ifndef DOWNWARD_HEURISTICS_IPDBS_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_IPDBS_HEURISTIC_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace downward::cli::heuristics {

language::plugins::InternalFunctionDefinitionBase&
add_ipdbs_heuristic_features(language::plugins::Namespace& nspace);

}

#endif