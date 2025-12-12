#ifndef PROBFD_CLI_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::pdbs {

void add_fully_additive_finder_factory_feature(
    language::plugins::Registry& registry);

}

#endif