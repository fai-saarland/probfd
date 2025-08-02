#ifndef PROBFD_CLI_PDBS_SUBCOLLECTION_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_SUBCOLLECTION_FINDER_FACTORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::pdbs {

void add_subcollection_finder_factory_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif