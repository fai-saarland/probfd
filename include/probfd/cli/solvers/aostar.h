#ifndef PROBFD_CLI_SOLVERS_AOSTAR_H
#define PROBFD_CLI_SOLVERS_AOSTAR_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::solvers {

void add_aostar_solver_features(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif