#ifndef PROBFD_CLI_SOLVERS_STATISTICAL_MDP_ALGORITHM_FACTORY_CATEGORY_H
#define PROBFD_CLI_SOLVERS_STATISTICAL_MDP_ALGORITHM_FACTORY_CATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_statistical_mdp_algorithm_factory_category(
    downward::cli::plugins::Registry& raw_registry);

}

#endif