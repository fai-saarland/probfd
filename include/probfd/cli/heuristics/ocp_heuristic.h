#ifndef PROBFD_CLI_HEURISTICS_OCP_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_OCP_HEURISTIC_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::heuristics {

void add_ocp_heuristic_feature(
    language::plugins::Registry& registry);

}

#endif