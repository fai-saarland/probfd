#include "downward/cli/plugins/plugin.h"

#include "probfd/heuristics/determinization_cost.h"

using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace downward::cli::plugins;

namespace {

class DeterminizationHeuristicFactoryFeature
    : public TypedFeature<
          TaskEvaluatorFactory,
          DeterminizationCostHeuristicFactory> {
public:
    DeterminizationHeuristicFactoryFeature()
        : TypedFeature("det")
    {
        document_title("Determinization-based Heuristic");
        document_synopsis("This heuristic returns the estimate of a classical "
                          "planning heuristic evaluated on the all-outcomes "
                          "determinization of the planning task.");

        add_option<std::shared_ptr<::Evaluator>>(
            "evaluator",
            "The classical planning heuristic.");
    }

    std::shared_ptr<DeterminizationCostHeuristicFactory>
    create_component(const Options& options, const Context&) const override
    {
        return std::make_shared<DeterminizationCostHeuristicFactory>(
            options.get<std::shared_ptr<::Evaluator>>("evaluator"));
    }
};

FeaturePlugin<DeterminizationHeuristicFactoryFeature> _plugin;

} // namespace
