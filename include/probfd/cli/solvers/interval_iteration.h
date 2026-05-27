#ifndef PROBFD_CLI_SOLVERS_INTERVAL_ITERATION_H
#define PROBFD_CLI_SOLVERS_INTERVAL_ITERATION_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_interval_iteration_solver_feature(
    downward::cli::plugins::Registry& registry);

}

#endif