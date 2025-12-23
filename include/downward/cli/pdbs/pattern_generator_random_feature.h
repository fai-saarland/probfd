#ifndef DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_RANDOM_FEATURE_H
#define DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_RANDOM_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace downward::cli::pdbs {

language::plugins::InternalFunctionDefinitionBase&
add_pattern_generator_random_feature(language::plugins::Namespace& nspace);
}

#endif