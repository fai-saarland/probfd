#ifndef PROBFD_CLI_HEURISTICS_UCP_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_UCP_HEURISTIC_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::heuristics {

void add_ucp_heuristic_feature(
    downward::cli::plugins::Registry& raw_registry);

}

#endif