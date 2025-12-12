#ifndef PROBFD_CLI_SOLVERS_ACYCLIC_VI_H
#define PROBFD_CLI_SOLVERS_ACYCLIC_VI_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_acyclic_value_iteration_feature(language::plugins::Registry& registry);

}

#endif