#include "probfd/cli/heuristics/determinization_cost_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/heuristics/determinization_cost_heuristic.h"

#include <downward/task_dependent_factory.h>

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace downward::cli::plugins;

namespace {
class DeterminizationHeuristicFactoryFeature
    : public SharedTypedFeature<
          TaskHeuristicFactory,
          std::shared_ptr<downward::TaskDependentFactory<Evaluator>>> {
public:
    DeterminizationHeuristicFactoryFeature()
        : TypedFeature("det", &DeterminizationHeuristicFactoryFeature::func)
    {
        document_title("Determinization-based Heuristic");
        document_synopsis(
            "This heuristic returns the estimate of a classical "
            "planning heuristic evaluated on the all-outcomes "
            "determinization of the planning task.");

        make_required_argument(
            0,
            "heuristic",
            "The classical planning heuristic.");
    }

    static std::shared_ptr<TaskHeuristicFactory> func(
        const Context&,
        std::shared_ptr<downward::TaskDependentFactory<Evaluator>> heuristic)
    {
        return std::make_shared<DeterminizationCostHeuristicFactory>(
            std::move(heuristic));
    }
};
} // namespace

namespace probfd::cli::heuristics {

void add_determinization_cost_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<DeterminizationHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
