#ifndef PROBFD_CLI_TRANSITION_SORTERS_SUBCATEGORY_H
#define PROBFD_CLI_TRANSITION_SORTERS_SUBCATEGORY_H

namespace language::plugins {
class Namespace;
}

namespace probfd::cli::transiton_sorters {

void add_transition_sorter_category(language::plugins::Namespace& nspace);

void add_transition_sorter_features(language::plugins::Namespace& nspace);

} // namespace probfd::cli::transiton_sorters

#endif