#ifndef PROBFD_CLI_SOLVERS_LRTDP_H
#define PROBFD_CLI_SOLVERS_LRTDP_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_lrtdp_features(
    downward::cli::plugins::Registry& registry);

}

#endif