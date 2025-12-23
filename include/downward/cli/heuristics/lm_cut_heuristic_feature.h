#ifndef DOWNWARD_HEURISTICS_LANDMARK_CUT_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_CUT_HEURISTIC_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace downward::cli::heuristics {

language::plugins::InternalFunctionDefinitionBase&
add_landmark_cut_heuristic_feature(language::plugins::Namespace& nspace);

}

#endif