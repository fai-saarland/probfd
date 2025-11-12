#ifndef PROBFD_CLI_SOLVERS_AOSTAR_H
#define PROBFD_CLI_SOLVERS_AOSTAR_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_aostar_solver_features(
    downward::cli::plugins::Registry& raw_registry);

}

#endif