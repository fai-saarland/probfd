#ifndef PROBFD_CLI_OPEN_LISTS_SUBCATEGORY_H
#define PROBFD_CLI_OPEN_LISTS_SUBCATEGORY_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::open_lists {

void add_open_list_features(
    language::plugins::RawRegistry& raw_registry);

}

#endif