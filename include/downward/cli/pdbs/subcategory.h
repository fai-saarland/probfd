#ifndef DOWNWARD_CLI_PDBS_SUBCATEGORY_H
#define DOWNWARD_CLI_PDBS_SUBCATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::pdbs {

void add_pdb_heuristic_subcategory(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif