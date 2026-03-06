#ifndef PROBFD_CLI_SOLVERS_EXHAUSTIVE_AO_H
#define PROBFD_CLI_SOLVERS_EXHAUSTIVE_AO_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::solvers {

void add_exhaustive_ao_solver_features(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif