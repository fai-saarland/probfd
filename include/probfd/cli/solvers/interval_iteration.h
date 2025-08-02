#ifndef PROBFD_CLI_SOLVERS_INTERVAL_ITERATION_H
#define PROBFD_CLI_SOLVERS_INTERVAL_ITERATION_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::solvers {

void add_interval_iteration_solver_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif