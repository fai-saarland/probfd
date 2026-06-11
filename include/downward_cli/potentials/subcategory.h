#ifndef DOWNWARD_CLI_POTENTIALS_SUBCATEGORY_H
#define DOWNWARD_CLI_POTENTIALS_SUBCATEGORY_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::potentials {

void add_potential_heuristics_subcategory(
    language::plugins::RawRegistry& raw_registry);

}

#endif