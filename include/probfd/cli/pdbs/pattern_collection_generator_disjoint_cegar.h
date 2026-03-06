#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::pdbs {

language::parser::InternalFunctionDefinitionBase&
add_pattern_collection_generator_disjoint_cegar_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif