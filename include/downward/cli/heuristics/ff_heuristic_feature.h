#ifndef DOWNWARD_HEURISTICS_FF_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_FF_HEURISTIC_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::heuristics {

language::plugins::InternalFunctionDefinitionBase&
add_ff_heuristic_features(language::plugins::Namespace& nspace);

}

#endif