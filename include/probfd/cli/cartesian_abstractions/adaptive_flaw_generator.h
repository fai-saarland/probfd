#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_ADAPTIVE_FLAW_GENERATOR_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_ADAPTIVE_FLAW_GENERATOR_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace probfd::cli::cartesian_abstractions {

language::plugins::InternalFunctionDefinitionBase&
add_adaptive_flaw_generator_feature(language::plugins::Namespace& nspace);

}

#endif