#include "probfd/cli/cartesian_abstractions/policy_based_flaw_generator.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/cartesian_abstractions/policy_based_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

namespace {
class ILAOFlawGeneratorFactoryFeature
    : public SharedTypedFeature<FlawGeneratorFactory> {
public:
    ILAOFlawGeneratorFactoryFeature()
        : TypedFeature("flaws_ilao")
    {
        add_optional_argument_with_default<probfd::value_t>(
            "convergence_epsilon",
            "10e-4",
            "The tolerance for convergence checks.");

        add_optional_argument_with_default<int>(
            "max_search_states",
            "infinity()",
            "maximum number of concrete states allowed to be generated during "
            "flaw "
            "search before giving up");
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

void add_policy_based_flaw_generator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<ILAOFlawGeneratorFactoryFeature>();
}

} // namespace probfd::cli::cartesian_abstractions