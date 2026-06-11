#ifndef PROBFD_CLI_HEURISTICS_MERGE_AND_SHRINK_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_MERGE_AND_SHRINK_HEURISTIC_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::heuristics {

void add_merge_and_shrink_heuristic_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif