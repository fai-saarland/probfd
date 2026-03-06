#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_SYSTEMATIC_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_SYSTEMATIC_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::pdbs {

language::parser::InternalFunctionDefinitionBase&
add_pattern_collection_generator_systematic_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif