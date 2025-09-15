#ifndef PROBFD_CLI_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::heuristics {

void add_additive_cartesian_heuristic_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif