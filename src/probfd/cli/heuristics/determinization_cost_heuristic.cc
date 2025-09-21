#include "probfd/cli/heuristics/determinization_cost_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/heuristics/determinization_cost_heuristic.h"

#include <downward/task_dependent_factory.h>

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace downward::cli::plugins;

namespace {
class DeterminizationHeuristicFactoryFeature
    : public TypedFeature<TaskHeuristicFactory> {
public:
    DeterminizationHeuristicFactoryFeature()
        : TypedFeature("det")
    {
        document_title("Determinization-based Heuristic");
        document_synopsis(
            "This heuristic returns the estimate of a classical "
            "planning heuristic evaluated on the all-outcomes "
            "determinization of the planning task.");

        add_option<std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>(
            "heuristic",
            "The classical planning heuristic.");
    }

    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options& options, const Context&) const override
    {
        return std::make_shared<DeterminizationCostHeuristicFactory>(
            options.get<
                std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>(
                "heuristic"));
    }
};
} // namespace

namespace probfd::cli::heuristics {

void add_determinization_cost_heuristic_feature(RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<DeterminizationHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
