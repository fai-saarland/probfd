#ifndef DOWNWARD_CLI_OPEN_LISTS_EPSILON_GREEDY_OPEN_LIST_FEATURE_H
#define DOWNWARD_CLI_OPEN_LISTS_EPSILON_GREEDY_OPEN_LIST_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::open_lists {

void add_epsilon_greedy_open_list_features(
    language::plugins::RawRegistry& raw_registry);

}

#endif