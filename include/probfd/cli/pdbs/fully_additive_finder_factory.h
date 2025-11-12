#ifndef PROBFD_CLI_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::pdbs {

void add_fully_additive_finder_factory_feature(
    downward::cli::plugins::Registry& raw_registry);

}

#endif