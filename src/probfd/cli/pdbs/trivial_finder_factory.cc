#include "downward/cli/plugins/plugin.h"

#include "probfd/pdbs/trivial_finder_factory.h"

using namespace utils;

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {

class TrivialFinderFactoryFeature
    : public TypedFeature<SubCollectionFinderFactory, TrivialFinderFactory> {
public:
    TrivialFinderFactoryFeature()
        : TypedFeature("finder_trivial_factory")
    {
    }

    std::shared_ptr<TrivialFinderFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<TrivialFinderFactory>();
    }
};

FeaturePlugin<TrivialFinderFactoryFeature> _plugin;

} // namespace
