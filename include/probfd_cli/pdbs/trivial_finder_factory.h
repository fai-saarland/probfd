#ifndef PROBFD_CLI_PDBS_TRIVIAL_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_TRIVIAL_FINDER_FACTORY_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::pdbs {

void add_trivial_finder_factory_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif