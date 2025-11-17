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
    : public SharedTypedFeature<
          FlawGeneratorFactory,
          std::vector<std::shared_ptr<FlawGeneratorFactory>>> {
public:
    AdaptiveFlawGeneratorFactoryFeature()
        : TypedFeature(
              "flaws_adaptive",
              &AdaptiveFlawGeneratorFactoryFeature::func)
    {
        make_optional_argument_with_default(
            0,
            "generators",
            "[flaws_astar(), flaws_ilao()]",
            "The linear hierarchy of flaw generators.");
    }

protected:
    static std::shared_ptr<FlawGeneratorFactory> func(
        const Context&,
        std::vector<std::shared_ptr<FlawGeneratorFactory>> generators)
    {
        return std::make_shared<AdaptiveFlawGeneratorFactory>(
            std::move(generators));
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