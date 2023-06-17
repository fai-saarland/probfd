#include "probfd/heuristics/pdbs/fully_additive_finder.h"
#include "probfd/heuristics/pdbs/fully_additive_finder_factory.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::unique_ptr<SubCollectionFinder>
FullyAdditiveFinderFactory::create_subcollection_finder(
    const ProbabilisticTaskProxy&)
{
    return std::make_unique<FullyAdditiveFinder>();
}

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

static plugins::FeaturePlugin<FullyAdditiveFinderFactoryFeature> _plugin;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd