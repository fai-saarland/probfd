#ifndef PROBFD_CLI_PDBS_TRIVIAL_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_TRIVIAL_FINDER_FACTORY_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::pdbs {

language::parser::InternalFunctionDefinitionBase&
add_trivial_finder_factory_feature(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace probfd::cli::pdbs

#endif