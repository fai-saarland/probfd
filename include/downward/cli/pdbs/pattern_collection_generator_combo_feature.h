#ifndef DOWNWARD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_COMBO_FEATURE_H
#define DOWNWARD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_COMBO_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::pdbs {

language::plugins::InternalFunctionDefinitionBase&
add_pattern_collection_generator_combo_feature(
    language::plugins::Namespace& nspace);

}

#endif