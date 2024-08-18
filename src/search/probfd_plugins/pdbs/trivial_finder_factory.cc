#include "probfd/pdbs/trivial_finder_factory.h"

#include "downward/plugins/plugin.h"

using namespace probfd::pdbs;

namespace {

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

plugins::FeaturePlugin<TrivialFinderFactoryFeature> _plugin;

} // namespace
