#ifndef PROBFD_CLI_HEURISTICS_UCP_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_UCP_HEURISTIC_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::heuristics {

void add_ucp_heuristic_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif