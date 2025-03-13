#include "downward/cli/plugins/plugin.h"

#include "probfd/solvers/statistical_mdp_algorithm.h"

using namespace probfd;
using namespace probfd::solvers;

using namespace downward::cli::plugins;

namespace {

class TaskSolverFactoryCategoryPlugin
    : public TypedCategoryPlugin<StatisticalMDPAlgorithmFactory> {
public:
    TaskSolverFactoryCategoryPlugin()
        : TypedCategoryPlugin("StatisticalMDPAlgorithmFactory")
    {
        document_synopsis(
            "Represents a factory that produces an MDP algorithm with "
            "statistics output for a given probabilistic planning task.");
    }
} _category_plugin;

} // namespace