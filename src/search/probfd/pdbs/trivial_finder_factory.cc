#include "probfd/pdbs/trivial_finder_factory.h"
#include "probfd/pdbs/trivial_finder.h"

#include "downward/plugins/plugin.h"

namespace probfd::pdbs {

std::unique_ptr<SubCollectionFinder>
TrivialFinderFactory::create_subcollection_finder(const ProbabilisticTaskProxy&)
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

    [[nodiscard]] std::shared_ptr<TrivialFinderFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<TrivialFinderFactory>();
    }
};

static plugins::FeaturePlugin<TrivialFinderFactoryFeature> _plugin;

} // namespace probfd::pdbs
