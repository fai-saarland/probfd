#ifndef PROBFD_CLI_SOLVERS_IDUAL_H
#define PROBFD_CLI_SOLVERS_IDUAL_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::solvers {

void add_idual_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif