#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_SEARCH_ALGORITHM_FACTORY_CATEGORY_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_SEARCH_ALGORITHM_FACTORY_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::search_algorithms {

void add_search_algorithm_factory_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif