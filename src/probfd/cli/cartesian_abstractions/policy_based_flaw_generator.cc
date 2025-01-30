#include "downward/cli/plugins/plugin.h"

#include "probfd/cartesian_abstractions/policy_based_flaw_generator.h"

using namespace utils;

using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

namespace {

class ILAOFlawGeneratorFactoryFeature
    : public TypedFeature<FlawGeneratorFactory, ILAOFlawGeneratorFactory> {
public:
    ILAOFlawGeneratorFactoryFeature()
        : TypedFeature("flaws_ilao")
    {
        add_option<probfd::value_t>(
            "convergence_epsilon",
            "The tolerance for convergence checks.",
            "10e-4");

        add_option<int>(
            "max_search_states",
            "maximum number of concrete states allowed to be generated during "
            "flaw "
            "search before giving up",
            "infinity",
            Bounds("1", "infinity"));
    }

    std::shared_ptr<ILAOFlawGeneratorFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return std::make_shared<ILAOFlawGeneratorFactory>(
            opts.get<probfd::value_t>("convergence_epsilon"),
            opts.get<int>("max_search_states"));
    }
};

FeaturePlugin<ILAOFlawGeneratorFactoryFeature> _plugin;

} // namespace