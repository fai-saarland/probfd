#ifndef PROBFD_CLI_SOLVERS_IDUAL_H
#define PROBFD_CLI_SOLVERS_IDUAL_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_idual_feature(
    downward::cli::plugins::Registry& raw_registry);

}

#endif