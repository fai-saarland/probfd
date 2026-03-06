#ifndef PROBFD_CLI_SOLVERS_LRTDP_H
#define PROBFD_CLI_SOLVERS_LRTDP_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::solvers {

void add_lrtdp_features(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif