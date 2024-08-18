#include "probfd/pdbs/max_orthogonal_finder_factory.h"

#include "downward/plugins/plugin.h"

using namespace probfd::pdbs;

namespace {

class AdditiveMaxOrthogonalityFinderFactoryFeature
    : public plugins::TypedFeature<
          SubCollectionFinderFactory,
          AdditiveMaxOrthogonalityFinderFactory> {
public:
    AdditiveMaxOrthogonalityFinderFactoryFeature()
        : TypedFeature("additive_max_orthogonality_factory")
    {
    }

    std::shared_ptr<AdditiveMaxOrthogonalityFinderFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<AdditiveMaxOrthogonalityFinderFactory>();
    }
};

class MultiplicativeMaxOrthogonalityFinderFactoryFeature
    : public plugins::TypedFeature<
          SubCollectionFinderFactory,
          MultiplicativeMaxOrthogonalityFinderFactory> {
public:
    MultiplicativeMaxOrthogonalityFinderFactoryFeature()
        : TypedFeature("multiplicative_max_orthogonality_factory")
    {
    }

    std::shared_ptr<MultiplicativeMaxOrthogonalityFinderFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<MultiplicativeMaxOrthogonalityFinderFactory>();
    }
};

plugins::FeaturePlugin<AdditiveMaxOrthogonalityFinderFactoryFeature> _plugin1;
plugins::FeaturePlugin<MultiplicativeMaxOrthogonalityFinderFactoryFeature>
    _plugin2;

} // namespace
