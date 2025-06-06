#include "downward/cli/plugins/plugin.h"

#include "probfd/cartesian_abstractions/trace_based_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace probfd::cartesian_abstractions;

using namespace downward::cli::plugins;

namespace {

class AStarFlawGeneratorFactoryFeature
    : public TypedFeature<FlawGeneratorFactory, AStarFlawGeneratorFactory> {
public:
    AStarFlawGeneratorFactoryFeature()
        : TypedFeature("flaws_astar")
    {
    }

    std::shared_ptr<AStarFlawGeneratorFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<AStarFlawGeneratorFactory>();
    }
};

FeaturePlugin<AStarFlawGeneratorFactoryFeature> _plugin;

} // namespace
