#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace probfd::cli::pdbs {

language::plugins::InternalFunctionDefinitionBase&
add_pattern_collection_generator_multiple_cegar_feature(
    language::plugins::Namespace& nspace);

}

#endif