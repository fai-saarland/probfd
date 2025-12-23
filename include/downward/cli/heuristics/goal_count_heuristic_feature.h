#ifndef DOWNWARD_HEURISTICS_GOAL_COUNT_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_GOAL_COUNT_HEURISTIC_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::heuristics {

language::plugins::InternalFunctionDefinitionBase&
add_goal_count_heuristic_features(language::plugins::Namespace& nspace);

}

#endif