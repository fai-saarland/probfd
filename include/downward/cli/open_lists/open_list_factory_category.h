#ifndef DOWNWARD_CLI_OPEN_LISTS_OPEN_LIST_FACTORY_CATEGORY_H
#define DOWNWARD_CLI_OPEN_LISTS_OPEN_LIST_FACTORY_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::open_lists {

void add_open_list_factory_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif