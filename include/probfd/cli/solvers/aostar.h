#ifndef PROBFD_CLI_SOLVERS_AOSTAR_H
#define PROBFD_CLI_SOLVERS_AOSTAR_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_aostar_solver_features(
    language::plugins::Registry& registry);

}

#endif