#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_POLICY_BASED_FLAW_GENERATOR_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_POLICY_BASED_FLAW_GENERATOR_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace probfd::cli::cartesian_abstractions {

language::plugins::InternalFunctionDefinitionBase&
add_policy_based_flaw_generator_feature(language::plugins::Namespace& nspace);

}

#endif