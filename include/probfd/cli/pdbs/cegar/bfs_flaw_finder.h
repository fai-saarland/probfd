#ifndef PROBFD_CLI_PDBS_CEGAR_BFS_FLAW_FINDER_H
#define PROBFD_CLI_PDBS_CEGAR_BFS_FLAW_FINDER_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::pdbs::cegar {

void add_bfs_flaw_finder_feature(
    language::plugins::Registry& registry);

}

#endif