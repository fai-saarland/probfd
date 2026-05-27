#ifndef PROBFD_CLI_HEURISTICS_DETERMINIZATION_COST_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_DETERMINIZATION_COST_HEURISTIC_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::heuristics {

void add_determinization_cost_heuristic_feature(
    downward::cli::plugins::Registry& registry);

}

#endif