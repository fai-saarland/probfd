#ifndef PROBFD_CLI_HEURISTICS_DETERMINIZATION_COST_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_DETERMINIZATION_COST_HEURISTIC_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::heuristics {

void add_determinization_cost_heuristic_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif