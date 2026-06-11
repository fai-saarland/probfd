#ifndef PROBFD_CLI_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::pdbs {

void add_fully_additive_finder_factory_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif