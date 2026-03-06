#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_ADAPTIVE_FLAW_GENERATOR_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_ADAPTIVE_FLAW_GENERATOR_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::cartesian_abstractions {

language::parser::InternalFunctionDefinitionBase&
add_adaptive_flaw_generator_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif