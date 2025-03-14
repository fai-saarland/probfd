#include "downward/cli/plugins/plugin.h"

#include "probfd/solvers/statistical_mdp_algorithm.h"

#include "probfd/bisimulation/types.h"

using namespace probfd;
using namespace probfd::solvers;

using namespace downward::cli::plugins;

namespace {

class TaskSolverFactoryCategoryPlugin
    : public TypedCategoryPlugin<FDRStatisticalMDPAlgorithmFactory> {
public:
    TaskSolverFactoryCategoryPlugin()
        : TypedCategoryPlugin("StatisticalMDPAlgorithmFactory")
    {
        document_synopsis(
            "Represents a factory that produces an MDP algorithm with "
            "statistics output for a given probabilistic planning task.");
    }
} _category_plugin;

class BTaskSolverFactoryCategoryPlugin
    : public TypedCategoryPlugin<StatisticalMDPAlgorithmFactory<
          bisimulation::QuotientState,
          OperatorID>> {
public:
    BTaskSolverFactoryCategoryPlugin()
        : TypedCategoryPlugin("BisimulationStatisticalMDPAlgorithmFactory")
    {
        document_synopsis(
            "Represents a factory that produces an MDP algorithm for a "
            "with statistics output for a bisimulation quotient.");
    }
} _category_plugin2;

} // namespace