#include "probfd/cli/cartesian_abstractions/adaptive_flaw_generator.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/cartesian_abstractions/adaptive_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

namespace {
class AdaptiveFlawGeneratorFactoryFeature
    : public TypedFeature<FlawGeneratorFactory> {
public:
    AdaptiveFlawGeneratorFactoryFeature()
        : TypedFeature("flaws_adaptive")
    {
        add_list_option<std::shared_ptr<FlawGeneratorFactory>>(
            "generators",
            "The linear hierarchy of flaw generators.",
            "[flaws_astar(), flaws_ilao()]");
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

void add_adaptive_flaw_generator_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<AdaptiveFlawGeneratorFactoryFeature>();
}

} // namespace probfd::cli::cartesian_abstractions