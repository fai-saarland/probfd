#ifndef PROBFD_CLI_HEURISTICS_CONSTANT_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_CONSTANT_HEURISTIC_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace probfd::cli::heuristics {

language::plugins::InternalFunctionDefinitionBase&
add_blind_heuristic_factory_feature(language::plugins::Namespace& nspace);

}

#endif