#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace probfd::cli::pdbs {

language::plugins::InternalFunctionDefinitionBase&
add_pattern_collection_generator_disjoint_cegar_feature(
    language::plugins::Namespace& nspace);

}

#endif