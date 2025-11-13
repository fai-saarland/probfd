#ifndef DOWNWARD_CLI_POTENTIALS_SUBCATEGORY_H
#define DOWNWARD_CLI_POTENTIALS_SUBCATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::potentials {

void add_potential_heuristics_subcategory(
    downward::cli::plugins::Registry& registry);

}

#endif