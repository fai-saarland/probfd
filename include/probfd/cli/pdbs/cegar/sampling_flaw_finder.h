#ifndef PROBFD_CLI_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H
#define PROBFD_CLI_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::pdbs::cegar {

void add_sampling_flaw_finder_feature(
    downward::cli::plugins::Registry& registry);

}

#endif