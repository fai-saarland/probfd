#ifndef PROBFD_CLI_SOLVERS_IDUAL_H
#define PROBFD_CLI_SOLVERS_IDUAL_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::solvers {

void add_idual_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif