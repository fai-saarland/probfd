#include "probfd/heuristics/determinization_cost.h"

#include "downward/plugins/plugin.h"

using namespace probfd;
using namespace probfd::heuristics;

namespace {

class DeterminizationHeuristicFactoryFeature
    : public plugins::TypedFeature<
          TaskEvaluatorFactory,
          DeterminizationCostHeuristicFactory> {
public:
    DeterminizationHeuristicFactoryFeature()
        : TypedFeature("det")
    {
        add_option<std::shared_ptr<::Evaluator>>("evaluator");
    }

    std::shared_ptr<DeterminizationCostHeuristicFactory>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        return std::make_shared<DeterminizationCostHeuristicFactory>(
            options.get<std::shared_ptr<::Evaluator>>("evaluator"));
    }
};

plugins::FeaturePlugin<DeterminizationHeuristicFactoryFeature> _plugin;

} // namespace
