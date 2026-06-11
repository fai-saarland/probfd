#ifndef PROBFD_CLI_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H
#define PROBFD_CLI_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::pdbs::cegar {

void add_sampling_flaw_finder_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif