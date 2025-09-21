#include "probfd/cli/pdbs/trivial_finder_factory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/pdbs/trivial_finder_factory.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {
class TrivialFinderFactoryFeature
    : public TypedFeature<SubCollectionFinderFactory> {
public:
    TrivialFinderFactoryFeature()
        : TypedFeature("finder_trivial_factory")
    {
    }

    std::shared_ptr<SubCollectionFinderFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<TrivialFinderFactory>();
    }
};
} // namespace

namespace probfd::cli::pdbs {

void add_trivial_finder_factory_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<TrivialFinderFactoryFeature>();
}

} // namespace probfd::cli::pdbs
