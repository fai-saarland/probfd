#ifndef PROBFD_CLI_SOLVERS_BISIMULATION_VI_H
#define PROBFD_CLI_SOLVERS_BISIMULATION_VI_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::solvers {

void add_bisimulation_value_iteration_features(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif