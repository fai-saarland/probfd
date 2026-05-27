#include "probfd/cli/cartesian_abstractions/adaptive_flaw_generator.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/cartesian_abstractions/adaptive_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

namespace {
class AdaptiveFlawGeneratorFactoryFeature
    : public SharedTypedFeature<FlawGeneratorFactory> {
public:
    AdaptiveFlawGeneratorFactoryFeature()
        : TypedFeature("flaws_adaptive")
    {
        add_optional_list_argument_with_default<
            std::shared_ptr<FlawGeneratorFactory>>(
            "generators",
            "[flaws_astar(), flaws_ilao()]",
            "The linear hierarchy of flaw generators.");
    }

protected:
    std::shared_ptr<FlawGeneratorFactory>
    create_component(const Options& options, const Context&) const override
    {
        return std::make_shared<AdaptiveFlawGeneratorFactory>(
            options.get_list<std::shared_ptr<FlawGeneratorFactory>>(
                "generators"));
    }
};
} // namespace

namespace probfd::cli::cartesian_abstractions {

void add_adaptive_flaw_generator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<AdaptiveFlawGeneratorFactoryFeature>();
}

} // namespace probfd::cli::cartesian_abstractions