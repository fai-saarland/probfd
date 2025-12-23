#ifndef DOWNWARD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_GENETIC_FEATURE_H
#define DOWNWARD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_GENETIC_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::pdbs {

language::plugins::InternalFunctionDefinitionBase&
add_pattern_collection_generator_genetic_feature(
    language::plugins::Namespace& nspace);

}

#endif