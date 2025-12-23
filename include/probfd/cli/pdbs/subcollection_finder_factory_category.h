#ifndef PROBFD_CLI_PDBS_SUBCOLLECTION_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_SUBCOLLECTION_FINDER_FACTORY_H

namespace language::plugins {
class Namespace;
}

namespace probfd::cli::pdbs {

void add_subcollection_finder_factory_category(
    language::plugins::Namespace& nspace);

}

#endif