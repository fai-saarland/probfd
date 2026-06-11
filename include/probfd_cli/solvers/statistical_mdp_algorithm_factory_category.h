#ifndef PROBFD_CLI_SOLVERS_STATISTICAL_MDP_ALGORITHM_FACTORY_CATEGORY_H
#define PROBFD_CLI_SOLVERS_STATISTICAL_MDP_ALGORITHM_FACTORY_CATEGORY_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::solvers {

void add_statistical_mdp_algorithm_factory_category(
    language::plugins::RawRegistry& raw_registry);

}

#endif