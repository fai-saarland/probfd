#ifndef PROBFD_CLI_SOLVERS_TOPOLOGICAL_VI_H
#define PROBFD_CLI_SOLVERS_TOPOLOGICAL_VI_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::solvers {

void add_topological_value_iteration_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif