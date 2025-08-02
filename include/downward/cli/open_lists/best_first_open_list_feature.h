#ifndef DOWNWARD_CLI_OPEN_LISTS_BEST_FIRST_OPEN_LIST_FEATURE_H
#define DOWNWARD_CLI_OPEN_LISTS_BEST_FIRST_OPEN_LIST_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::open_lists {

void add_best_first_open_list_features(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif