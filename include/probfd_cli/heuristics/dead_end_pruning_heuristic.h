#ifndef PROBFD_CLI_HEURISTICS_DEAD_END_PRUNING_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_DEAD_END_PRUNING_HEURISTIC_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::heuristics {

void add_dead_end_pruning_heuristic_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif