#ifndef PROBFD_CLI_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::heuristics {

void add_additive_cartesian_heuristic_feature(
    language::plugins::Registry& registry);

}

#endif