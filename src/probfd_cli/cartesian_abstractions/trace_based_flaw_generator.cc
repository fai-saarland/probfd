#include "probfd_cli/cartesian_abstractions/trace_based_flaw_generator.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/cartesian_abstractions/trace_based_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace probfd::cartesian_abstractions;

using namespace language;
using namespace language::plugins;

namespace {
class AStarFlawGeneratorFactoryFeature
    : public TypedFeature<FlawGeneratorFactory> {
public:
    AStarFlawGeneratorFactoryFeature()
        : TypedFeature("flaws_astar")
    {
    }

    std::shared_ptr<FlawGeneratorFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<AStarFlawGeneratorFactory>();
    }
};
} // namespace

namespace probfd::cli::cartesian_abstractions {

void add_astar_flaw_generator_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<AStarFlawGeneratorFactoryFeature>();
}

} // namespace probfd::cli::cartesian_abstractions
