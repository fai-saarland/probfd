#include "probfd/cli/pdbs/fully_additive_finder_factory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/pdbs/fully_additive_finder_factory.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {
class FullyAdditiveFinderFactoryFeature
    : public TypedFeature<
          SubCollectionFinderFactory,
          FullyAdditiveFinderFactory> {
public:
    FullyAdditiveFinderFactoryFeature()
        : TypedFeature("fully_additive_factory")
    {
    }

    std::shared_ptr<FullyAdditiveFinderFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<FullyAdditiveFinderFactory>();
    }
};
}

namespace probfd::cli::pdbs {

void add_fully_additive_finder_factory_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<FullyAdditiveFinderFactoryFeature>();
}

} // namespace
