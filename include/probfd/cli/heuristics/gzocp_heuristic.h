#ifndef PROBFD_CLI_HEURISTICS_GZOCP_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_GZOCP_HEURISTIC_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::heuristics {

void add_gzocp_heuristic_feature(
    language::plugins::Registry& registry);

}

#endif