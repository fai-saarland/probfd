#include "probfd/pdbs/fully_additive_finder_factory.h"

#include "downward/plugins/plugin.h"

using namespace probfd::pdbs;

namespace {

class FullyAdditiveFinderFactoryFeature
    : public plugins::
          TypedFeature<SubCollectionFinderFactory, FullyAdditiveFinderFactory> {
public:
    FullyAdditiveFinderFactoryFeature()
        : TypedFeature("fully_additive_factory")
    {
    }

    std::shared_ptr<FullyAdditiveFinderFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<FullyAdditiveFinderFactory>();
    }
};

plugins::FeaturePlugin<FullyAdditiveFinderFactoryFeature> _plugin;

} // namespace
