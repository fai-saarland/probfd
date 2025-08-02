#ifndef PROBFD_CLI_TRANSITION_SORTERS_SUBCATEGORY_H
#define PROBFD_CLI_TRANSITION_SORTERS_SUBCATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::transiton_sorters {

void add_transition_sorter_features(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif