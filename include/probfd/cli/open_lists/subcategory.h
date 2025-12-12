#ifndef PROBFD_CLI_OPEN_LISTS_SUBCATEGORY_H
#define PROBFD_CLI_OPEN_LISTS_SUBCATEGORY_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::open_lists {

void add_open_list_categories(
    language::plugins::Registry& registry);

void add_open_list_features(
    language::plugins::Registry& registry);

}

#endif