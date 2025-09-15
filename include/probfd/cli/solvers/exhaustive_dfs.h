#ifndef PROBFD_CLI_SOLVERS_EXHAUSTIVE_DFS_H
#define PROBFD_CLI_SOLVERS_EXHAUSTIVE_DFS_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::solvers {

void add_exhaustive_dfs_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif