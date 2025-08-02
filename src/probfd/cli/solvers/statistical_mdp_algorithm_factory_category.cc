#include "probfd/cli/solvers/statistical_mdp_algorithm_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/solvers/statistical_mdp_algorithm.h"

using namespace probfd;
using namespace probfd::solvers;

using namespace downward::cli::plugins;

namespace {
class StatisticalMDPAlgorithmFactoryCategoryPlugin
    : public TypedCategoryPlugin<StatisticalMDPAlgorithmFactory> {
public:
    StatisticalMDPAlgorithmFactoryCategoryPlugin()
        : TypedCategoryPlugin("StatisticalMDPAlgorithmFactory")
    {
        document_synopsis(
            "Represents a factory that produces an MDP algorithm with "
            "statistics output for a given probabilistic planning task.");
    }
};
} // namespace

namespace probfd::cli::solvers {

void add_statistical_mdp_algorithm_factory_category(RawRegistry& raw_registry)
{
    raw_registry
        .insert_category_plugin<StatisticalMDPAlgorithmFactoryCategoryPlugin>();
}

} // namespace probfd::cli::solvers