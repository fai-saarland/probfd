#include "probfd/cli/pdbs/trivial_finder_factory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/pdbs/trivial_finder_factory.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {
class TrivialFinderFactoryFeature
    : public SharedTypedFeature<SubCollectionFinderFactory> {
public:
    TrivialFinderFactoryFeature()
        : TypedFeature(
              "finder_trivial_factory",
              &TrivialFinderFactoryFeature::func)
    {
    }

    static std::shared_ptr<SubCollectionFinderFactory> func(const Context&)
    {
        return std::make_shared<TrivialFinderFactory>();
    }
};
} // namespace

namespace probfd::cli::pdbs {

void add_trivial_finder_factory_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<TrivialFinderFactoryFeature>();
}

} // namespace probfd::cli::pdbs
