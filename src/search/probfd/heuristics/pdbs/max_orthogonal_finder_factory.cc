#include "probfd/heuristics/pdbs/max_orthogonal_finder_factory.h"
#include "probfd/heuristics/pdbs/max_orthogonal_finder.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::unique_ptr<SubCollectionFinder>
AdditiveMaxOrthogonalityFinderFactory::create_subcollection_finder(
    const ProbabilisticTaskProxy& task_proxy)
{
    return std::make_unique<AdditiveMaxOrthogonalityFinder>(task_proxy);
}

std::unique_ptr<SubCollectionFinder>
MultiplicativeMaxOrthogonalityFinderFactory::create_subcollection_finder(
    const ProbabilisticTaskProxy& task_proxy)
{
    return std::make_unique<MultiplicativeMaxOrthogonalityFinder>(task_proxy);
}

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

static plugins::FeaturePlugin<AdditiveMaxOrthogonalityFinderFactoryFeature>
    _plugin1;
static plugins::FeaturePlugin<
    MultiplicativeMaxOrthogonalityFinderFactoryFeature>
    _plugin2;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd