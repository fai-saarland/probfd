#ifndef PROBFD_CLI_TRANSITION_SORTERS_SUBCATEGORY_H
#define PROBFD_CLI_TRANSITION_SORTERS_SUBCATEGORY_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::transiton_sorters {

void add_transition_sorter_category(
    language::plugins::Registry& registry);

void add_transition_sorter_features(
    language::plugins::Registry& registry);

} // namespace probfd::cli::transiton_sorters

#endif