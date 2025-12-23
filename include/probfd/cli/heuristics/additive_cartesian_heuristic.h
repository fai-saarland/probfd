#ifndef PROBFD_CLI_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace probfd::cli::heuristics {

language::plugins::InternalFunctionDefinitionBase&
add_additive_cartesian_heuristic_feature(language::plugins::Namespace& nspace);

}

#endif