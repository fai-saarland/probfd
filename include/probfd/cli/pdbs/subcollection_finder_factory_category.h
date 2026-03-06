#ifndef PROBFD_CLI_PDBS_SUBCOLLECTION_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_SUBCOLLECTION_FINDER_FACTORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::pdbs {

void add_subcollection_finder_factory_category(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif