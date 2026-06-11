#ifndef PROBFD_CLI_SOLVERS_DEPTH_FIRST_HEURISTIC_SEARCH_H
#define PROBFD_CLI_SOLVERS_DEPTH_FIRST_HEURISTIC_SEARCH_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::solvers {

void add_depth_first_heuristic_search_features(
    language::plugins::RawRegistry& raw_registry);

}

#endif