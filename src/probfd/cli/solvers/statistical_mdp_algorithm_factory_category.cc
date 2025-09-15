#include "probfd/cli/solvers/statistical_mdp_algorithm_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/solvers/statistical_mdp_algorithm.h"

using namespace probfd::solvers;

using namespace downward::cli::plugins;

namespace probfd::cli::solvers {

void add_statistical_mdp_algorithm_factory_category(RawRegistry& raw_registry)
{
    auto& category =
        raw_registry.insert_category_plugin<StatisticalMDPAlgorithmFactory>(
            "StatisticalMDPAlgorithmFactory");
    category.document_synopsis(
        "Represents a factory that produces an MDP algorithm with "
        "statistics output for a given probabilistic planning task.");
}

} // namespace probfd::cli::solvers