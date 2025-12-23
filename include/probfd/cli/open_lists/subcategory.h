#ifndef PROBFD_CLI_OPEN_LISTS_SUBCATEGORY_H
#define PROBFD_CLI_OPEN_LISTS_SUBCATEGORY_H

namespace language::plugins {
class Namespace;
}

namespace probfd::cli::open_lists {

void add_open_list_categories(language::plugins::Namespace& nspace);

void add_open_list_features(language::plugins::Namespace& nspace);

} // namespace probfd::cli::open_lists

#endif