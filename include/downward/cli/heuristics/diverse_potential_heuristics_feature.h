#ifndef DOWNWARD_HEURISTICS_DIVERSE_POTENTIAL_HEURISTICS_FEATURE_H
#define DOWNWARD_HEURISTICS_DIVERSE_POTENTIAL_HEURISTICS_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::heuristics {

language::plugins::InternalFunctionDefinitionBase&
add_diverse_potential_heuristics_feature(language::plugins::Namespace& nspace);

}

#endif