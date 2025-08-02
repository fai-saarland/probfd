#ifndef PROBFD_CLI_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::pdbs {

void add_fully_additive_finder_factory_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif