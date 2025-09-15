#ifndef PROBFD_CLI_SOLVERS_LRTDP_H
#define PROBFD_CLI_SOLVERS_LRTDP_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::solvers {

void add_lrtdp_features(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif