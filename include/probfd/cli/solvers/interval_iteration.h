#ifndef PROBFD_CLI_SOLVERS_INTERVAL_ITERATION_H
#define PROBFD_CLI_SOLVERS_INTERVAL_ITERATION_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::solvers {

void add_interval_iteration_solver_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif