#ifndef PROBFD_CLI_OCCUPATION_MEASURES_CONSTRAINT_GENERATOR_FACTORY_CATEGORY_H
#define PROBFD_CLI_OCCUPATION_MEASURES_CONSTRAINT_GENERATOR_FACTORY_CATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::occupation_measures {

void add_constraint_generator_factory_category(
    downward::cli::plugins::Registry& registry);

}

#endif