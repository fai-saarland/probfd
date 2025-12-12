#ifndef PROBFD_CLI_HEURISTICS_CONSTANT_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_CONSTANT_HEURISTIC_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::heuristics {

void add_blind_heuristic_factory_feature(
    language::plugins::Registry& registry);

}

#endif