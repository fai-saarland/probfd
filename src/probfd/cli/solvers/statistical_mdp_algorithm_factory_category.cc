#include "probfd/cli/solvers/statistical_mdp_algorithm_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/solvers/statistical_mdp_algorithm.h"

using namespace probfd::solvers;

using namespace downward::cli::plugins;

namespace probfd::cli::solvers {

void add_statistical_mdp_algorithm_factory_category(Registry& raw_registry)
{
    raw_registry.insert_shared_category_plugin<StatisticalMDPAlgorithmFactory>(
        "StatisticalMDPAlgorithmFactory",
        "Represents a factory that produces an MDP algorithm with "
        "statistics output for a given probabilistic planning task.");
}

} // namespace probfd::cli::solvers