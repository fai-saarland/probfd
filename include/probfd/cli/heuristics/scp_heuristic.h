#ifndef PROBFD_CLI_HEURISTICS_SCP_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_SCP_HEURISTIC_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::heuristics {

void add_scp_heuristic_feature(
    language::plugins::Registry& registry);

}

#endif