#ifndef PROBFD_CLI_SOLVERS_ACYCLIC_VI_H
#define PROBFD_CLI_SOLVERS_ACYCLIC_VI_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::solvers {

void add_acyclic_value_iteration_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif