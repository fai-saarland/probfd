#include "probfd/cartesian_abstractions/adaptive_flaw_generator.h"

#include "downward/plugins/plugin.h"

using namespace probfd::cartesian_abstractions;

namespace {

class AdaptiveFlawGeneratorFactoryFeature
    : public plugins::
          TypedFeature<FlawGeneratorFactory, AdaptiveFlawGeneratorFactory> {
public:
    AdaptiveFlawGeneratorFactoryFeature()
        : TypedFeature("flaws_adaptive")
    {
        add_list_option<std::shared_ptr<FlawGeneratorFactory>>(
            "generators",
            "The linear hierachy of flaw generators.",
            "[flaws_astar(), flaws_ilao()]");
    }

protected:
    std::shared_ptr<AdaptiveFlawGeneratorFactory>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        return std::make_shared<AdaptiveFlawGeneratorFactory>(
            options.get_list<std::shared_ptr<FlawGeneratorFactory>>(
                "generators"));
    }
};

plugins::FeaturePlugin<AdaptiveFlawGeneratorFactoryFeature> _plugin;

} // namespace