#include "probfd/cartesian_abstractions/trace_based_flaw_generator.h"

#include "downward/plugins/plugin.h"

using namespace probfd::cartesian_abstractions;

namespace {

class AStarFlawGeneratorFactoryFeature
    : public plugins::
          TypedFeature<FlawGeneratorFactory, AStarFlawGeneratorFactory> {
public:
    AStarFlawGeneratorFactoryFeature()
        : TypedFeature("flaws_astar")
    {
    }

    std::shared_ptr<AStarFlawGeneratorFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<AStarFlawGeneratorFactory>();
    }
};

plugins::FeaturePlugin<AStarFlawGeneratorFactoryFeature> _plugin;

} // namespace
