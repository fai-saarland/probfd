#ifndef DOWNWARD_HEURISTICS_OPERATOR_COUNTING_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_OPERATOR_COUNTING_HEURISTIC_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::heuristics {

language::plugins::InternalFunctionDefinitionBase&
add_operator_counting_heuristic_feature(language::plugins::Namespace& nspace);

}

#endif