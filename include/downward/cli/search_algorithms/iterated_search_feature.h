#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_ITERATED_SEARCH_FEATURE_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_ITERATED_SEARCH_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::search_algorithms {

void add_iterated_search_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif