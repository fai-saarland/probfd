#ifndef PROBFD_CLI_SOLVERS_LRTDP_H
#define PROBFD_CLI_SOLVERS_LRTDP_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_lrtdp_features(
    language::plugins::Registry& registry);

}

#endif