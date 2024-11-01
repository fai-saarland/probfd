#include "probfd/heuristics/dead_end_pruning.h"

#include "downward/plugins/plugin.h"

using namespace probfd;
using namespace probfd::heuristics;

namespace {

class DeadEndPruningHeuristicFactoryFeature
    : public plugins::
          TypedFeature<TaskEvaluatorFactory, DeadEndPruningHeuristicFactory> {
public:
    DeadEndPruningHeuristicFactoryFeature()
        : TypedFeature("prune_dead_ends")
    {
        add_option<std::shared_ptr<::Evaluator>>("evaluator");
    }

protected:
    std::shared_ptr<DeadEndPruningHeuristicFactory>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        return std::make_shared<DeadEndPruningHeuristicFactory>(
            options.get<std::shared_ptr<::Evaluator>>("evaluator"));
    }
};

plugins::FeaturePlugin<DeadEndPruningHeuristicFactoryFeature> _plugin;

} // namespace
