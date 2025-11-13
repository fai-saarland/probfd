#ifndef PROBFD_CLI_HEURISTICS_DEAD_END_PRUNING_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_DEAD_END_PRUNING_HEURISTIC_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::heuristics {

void add_dead_end_pruning_heuristic_feature(
    downward::cli::plugins::Registry& registry);

}

#endif