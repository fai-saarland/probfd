#include "probfd/cartesian_abstractions/policy_based_flaw_generator.h"

#include "downward/plugins/plugin.h"

using namespace probfd::cartesian_abstractions;

namespace {

class ILAOFlawGeneratorFactoryFeature
    : public plugins::
          TypedFeature<FlawGeneratorFactory, ILAOFlawGeneratorFactory> {
public:
    ILAOFlawGeneratorFactoryFeature()
        : TypedFeature("flaws_ilao")
    {
        add_option<int>(
            "max_search_states",
            "maximum number of concrete states allowed to be generated during "
            "flaw "
            "search before giving up",
            "infinity",
            plugins::Bounds("1", "infinity"));
    }

    std::shared_ptr<ILAOFlawGeneratorFactory>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return std::make_shared<ILAOFlawGeneratorFactory>(
            opts.get<int>("max_search_states"));
    }
};

plugins::FeaturePlugin<ILAOFlawGeneratorFactoryFeature> _plugin;

} // namespace