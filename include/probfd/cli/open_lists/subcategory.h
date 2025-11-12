#ifndef PROBFD_CLI_OPEN_LISTS_SUBCATEGORY_H
#define PROBFD_CLI_OPEN_LISTS_SUBCATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::open_lists {

void add_open_list_categories(
    downward::cli::plugins::Registry& raw_registry);

void add_open_list_features(
    downward::cli::plugins::Registry& raw_registry);

}

#endif