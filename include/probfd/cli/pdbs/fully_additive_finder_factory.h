#ifndef PROBFD_CLI_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::pdbs {

language::parser::InternalFunctionDefinitionBase&
add_fully_additive_finder_factory_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif