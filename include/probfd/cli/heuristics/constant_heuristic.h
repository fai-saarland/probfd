#ifndef PROBFD_CLI_HEURISTICS_CONSTANT_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_CONSTANT_HEURISTIC_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::heuristics {

void add_blind_heuristic_factory_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif