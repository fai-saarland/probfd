#include "downward_plugins/plugins/plugin.h"

#include "probfd/heuristics/determinization_cost.h"

using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace downward_plugins::plugins;

namespace {

class DeterminizationHeuristicFactoryFeature
    : public TypedFeature<
          TaskEvaluatorFactory,
          DeterminizationCostHeuristicFactory> {
public:
    DeterminizationHeuristicFactoryFeature()
        : TypedFeature("det")
    {
        add_option<std::shared_ptr<::Evaluator>>("evaluator");
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
