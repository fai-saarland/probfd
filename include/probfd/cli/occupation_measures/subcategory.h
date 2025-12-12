#ifndef PROBFD_CLI_OCCUPATION_MEASURES_SUBCATEGORY_H
#define PROBFD_CLI_OCCUPATION_MEASURES_SUBCATEGORY_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::occupation_measures {

void add_occupation_measure_heuristics_features(
    language::plugins::Registry& registry);

}

#endif