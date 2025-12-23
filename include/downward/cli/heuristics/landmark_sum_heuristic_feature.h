#ifndef DOWNWARD_HEURISTICS_LANDMARK_SUM_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_LANDMARK_SUM_HEURISTIC_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::heuristics {

language::plugins::InternalFunctionDefinitionBase&
add_landmark_sum_heuristic_feature(language::plugins::Namespace& nspace);

}

#endif