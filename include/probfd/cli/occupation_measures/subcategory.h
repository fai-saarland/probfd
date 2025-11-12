#ifndef PROBFD_CLI_OCCUPATION_MEASURES_SUBCATEGORY_H
#define PROBFD_CLI_OCCUPATION_MEASURES_SUBCATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::occupation_measures {

void add_occupation_measure_heuristics_features(
    downward::cli::plugins::Registry& raw_registry);

}

#endif