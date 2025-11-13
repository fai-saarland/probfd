#ifndef PROBFD_CLI_TRANSITION_SORTERS_SUBCATEGORY_H
#define PROBFD_CLI_TRANSITION_SORTERS_SUBCATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::transiton_sorters {

void add_transition_sorter_category(
    downward::cli::plugins::Registry& registry);

void add_transition_sorter_features(
    downward::cli::plugins::Registry& registry);

} // namespace probfd::cli::transiton_sorters

#endif