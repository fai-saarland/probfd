#include "probfd/cli/cartesian_abstractions/policy_based_flaw_generator.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/cartesian_abstractions/policy_based_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

namespace {
class ILAOFlawGeneratorFactoryFeature
    : public TypedFeature<FlawGeneratorFactory> {
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

    std::shared_ptr<FlawGeneratorFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return std::make_shared<ILAOFlawGeneratorFactory>(
            opts.get<probfd::value_t>("convergence_epsilon"),
            opts.get<int>("max_search_states"));
    }
};
} // namespace

namespace probfd::cli::cartesian_abstractions {

void add_policy_based_flaw_generator_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<ILAOFlawGeneratorFactoryFeature>();
}

} // namespace probfd::cli::cartesian_abstractions