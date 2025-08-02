#ifndef PROBFD_CLI_PDBS_MAX_ORTHOGONAL_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_MAX_ORTHOGONAL_FINDER_FACTORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::pdbs {

void add_max_orthogonal_finder_factory_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif