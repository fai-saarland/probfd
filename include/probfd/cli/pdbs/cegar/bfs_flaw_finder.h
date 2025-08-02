#ifndef PROBFD_CLI_PDBS_CEGAR_BFS_FLAW_FINDER_H
#define PROBFD_CLI_PDBS_CEGAR_BFS_FLAW_FINDER_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::pdbs::cegar {

void add_bfs_flaw_finder_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif