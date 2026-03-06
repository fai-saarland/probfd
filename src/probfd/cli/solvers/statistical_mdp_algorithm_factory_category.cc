#include "probfd/cli/solvers/statistical_mdp_algorithm_factory_category.h"

#include "language/ast/internal_type_declaration.h"

#include "probfd/solvers/statistical_mdp_algorithm.h"

using namespace probfd::solvers;

using namespace language::parser;

namespace probfd::cli::solvers {

void add_statistical_mdp_algorithm_factory_category(
    NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<StatisticalMDPAlgorithmFactory>(
        nspace,
        "StatisticalMDPAlgorithmFactory",
        "Represents a factory that produces an MDP algorithm with "
        "statistics output for a given probabilistic planning task.");
}

} // namespace probfd::cli::solvers