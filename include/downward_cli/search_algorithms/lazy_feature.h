#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_LAZY_FEATURE_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_LAZY_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::search_algorithms {

void add_lazy_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif