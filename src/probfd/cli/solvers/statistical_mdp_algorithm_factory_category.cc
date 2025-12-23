#include "probfd/cli/solvers/statistical_mdp_algorithm_factory_category.h"

#include "language/plugins/registry.h"

#include "probfd/solvers/statistical_mdp_algorithm.h"

using namespace probfd::solvers;

using namespace language::plugins;

namespace probfd::cli::solvers {

void add_statistical_mdp_algorithm_factory_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<StatisticalMDPAlgorithmFactory>(
        "StatisticalMDPAlgorithmFactory",
        "Represents a factory that produces an MDP algorithm with "
        "statistics output for a given probabilistic planning task.");
}

} // namespace probfd::cli::solvers