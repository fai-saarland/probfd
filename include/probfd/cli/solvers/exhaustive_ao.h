#ifndef PROBFD_CLI_SOLVERS_EXHAUSTIVE_AO_H
#define PROBFD_CLI_SOLVERS_EXHAUSTIVE_AO_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::solvers {

void add_exhaustive_ao_solver_features(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif