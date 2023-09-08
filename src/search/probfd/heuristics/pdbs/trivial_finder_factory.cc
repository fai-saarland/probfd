#include "probfd/heuristics/pdbs/trivial_finder_factory.h"
#include "probfd/heuristics/pdbs/trivial_finder.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::unique_ptr<SubCollectionFinder>
TrivialFinderFactory::create_subcollection_finder(ProbabilisticTaskProxy)
{
    return std::make_unique<TrivialFinder>();
}

class TrivialFinderFactoryFeature
    : public plugins::
          TypedFeature<SubCollectionFinderFactory, TrivialFinderFactory> {
public:
    TrivialFinderFactoryFeature()
        : TypedFeature("finder_trivial_factory")
    {
    }

    std::shared_ptr<TrivialFinderFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<TrivialFinderFactory>();
    }
};

static plugins::FeaturePlugin<TrivialFinderFactoryFeature> _plugin;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd