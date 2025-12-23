#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace probfd::cli::pdbs {

language::plugins::InternalFunctionDefinitionBase&
add_pattern_collection_generator_hillclimbing_feature(
    language::plugins::Namespace& nspace);

}

#endif