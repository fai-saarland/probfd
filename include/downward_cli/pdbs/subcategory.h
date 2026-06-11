#ifndef DOWNWARD_CLI_PDBS_SUBCATEGORY_H
#define DOWNWARD_CLI_PDBS_SUBCATEGORY_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::pdbs {

void add_pdb_heuristic_subcategory(
    language::plugins::RawRegistry& raw_registry);

}

#endif