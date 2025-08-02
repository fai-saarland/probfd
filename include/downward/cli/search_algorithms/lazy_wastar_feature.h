#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_LAZY_WASTAR_FEATURE_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_LAZY_WASTAR_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::search_algorithms {

void add_lazy_wastar_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif