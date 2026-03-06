#ifndef PROBFD_CLI_SOLVERS_EXHAUSTIVE_DFS_H
#define PROBFD_CLI_SOLVERS_EXHAUSTIVE_DFS_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::solvers {

void add_exhaustive_dfs_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif