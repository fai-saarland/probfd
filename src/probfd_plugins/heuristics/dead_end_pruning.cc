#include "downward_plugins/plugins/plugin.h"

#include "probfd/heuristics/dead_end_pruning.h"

using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace downward_plugins::plugins;

namespace {

class DeadEndPruningHeuristicFactoryFeature
    : public TypedFeature<
          TaskEvaluatorFactory,
          DeadEndPruningHeuristicFactory> {
public:
    DeadEndPruningHeuristicFactoryFeature()
        : TypedFeature("prune_dead_ends")
    {
        add_option<std::shared_ptr<::Evaluator>>("evaluator");
    }

protected:
    std::shared_ptr<DeadEndPruningHeuristicFactory>
    create_component(const Options& options, const Context&) const override
    {
        return std::make_shared<DeadEndPruningHeuristicFactory>(
            options.get<std::shared_ptr<::Evaluator>>("evaluator"));
    }
};

FeaturePlugin<DeadEndPruningHeuristicFactoryFeature> _plugin;

} // namespace
