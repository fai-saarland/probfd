#include "downward/cli/plugins/plugin.h"

#include "probfd/pdbs/max_orthogonal_finder_factory.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {

class AdditiveMaxOrthogonalityFinderFactoryFeature
    : public TypedFeature<
          SubCollectionFinderFactory,
          AdditiveMaxOrthogonalityFinderFactory> {
public:
    AdditiveMaxOrthogonalityFinderFactoryFeature()
        : TypedFeature("additive_max_orthogonality_factory")
    {
    }

    std::shared_ptr<AdditiveMaxOrthogonalityFinderFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<AdditiveMaxOrthogonalityFinderFactory>();
    }
};

class MultiplicativeMaxOrthogonalityFinderFactoryFeature
    : public TypedFeature<
          SubCollectionFinderFactory,
          MultiplicativeMaxOrthogonalityFinderFactory> {
public:
    MultiplicativeMaxOrthogonalityFinderFactoryFeature()
        : TypedFeature("multiplicative_max_orthogonality_factory")
    {
    }

    std::shared_ptr<MultiplicativeMaxOrthogonalityFinderFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<MultiplicativeMaxOrthogonalityFinderFactory>();
    }
};

FeaturePlugin<AdditiveMaxOrthogonalityFinderFactoryFeature> _plugin1;
FeaturePlugin<MultiplicativeMaxOrthogonalityFinderFactoryFeature> _plugin2;

} // namespace
