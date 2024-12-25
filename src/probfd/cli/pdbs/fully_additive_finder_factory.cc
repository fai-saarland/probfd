#include "downward/cli/plugins/plugin.h"

#include "probfd/pdbs/fully_additive_finder_factory.h"

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

FeaturePlugin<FullyAdditiveFinderFactoryFeature> _plugin;

} // namespace
