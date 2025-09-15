#ifndef PROBFD_CLI_HEURISTICS_SCP_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_SCP_HEURISTIC_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::heuristics {

void add_scp_heuristic_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif