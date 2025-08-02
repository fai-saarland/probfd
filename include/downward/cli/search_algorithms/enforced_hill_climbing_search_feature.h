#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_ENFORCED_HILL_CLIMBING_SEARCH_FEATURE_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_ENFORCED_HILL_CLIMBING_SEARCH_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::search_algorithms {

void add_enforce_hill_climbing_search_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif