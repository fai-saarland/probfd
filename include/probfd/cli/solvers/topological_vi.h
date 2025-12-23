#ifndef PROBFD_CLI_SOLVERS_TOPOLOGICAL_VI_H
#define PROBFD_CLI_SOLVERS_TOPOLOGICAL_VI_H

namespace language::plugins {
class Namespace;
}

namespace probfd::cli::solvers {

void add_topological_value_iteration_feature(
    language::plugins::Namespace& nspace);

}

#endif