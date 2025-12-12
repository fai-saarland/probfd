#ifndef PROBFD_CLI_SOLVERS_EXHAUSTIVE_DFS_H
#define PROBFD_CLI_SOLVERS_EXHAUSTIVE_DFS_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_exhaustive_dfs_feature(
    language::plugins::Registry& registry);

}

#endif