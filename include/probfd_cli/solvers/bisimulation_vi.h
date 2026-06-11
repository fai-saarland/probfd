#ifndef PROBFD_CLI_SOLVERS_BISIMULATION_VI_H
#define PROBFD_CLI_SOLVERS_BISIMULATION_VI_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::solvers {

void add_bisimulation_value_iteration_features(
    language::plugins::RawRegistry& raw_registry);

}

#endif