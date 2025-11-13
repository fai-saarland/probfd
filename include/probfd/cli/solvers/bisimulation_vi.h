#ifndef PROBFD_CLI_SOLVERS_BISIMULATION_VI_H
#define PROBFD_CLI_SOLVERS_BISIMULATION_VI_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_bisimulation_value_iteration_features(
    downward::cli::plugins::Registry& registry);

}

#endif