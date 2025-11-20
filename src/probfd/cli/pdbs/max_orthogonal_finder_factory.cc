#include "probfd/cli/pdbs/max_orthogonal_finder_factory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/pdbs/max_orthogonal_finder_factory.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {
class AdditiveMaxOrthogonalityFinderFactoryFeature
    : public SharedTypedFeature<SubCollectionFinderFactory> {
public:
    AdditiveMaxOrthogonalityFinderFactoryFeature()
        : TypedFeature(
              "additive_max_orthogonality_factory",
              &AdditiveMaxOrthogonalityFinderFactoryFeature::func)
    {
    }

    static std::shared_ptr<SubCollectionFinderFactory> func()
    {
        return std::make_shared<AdditiveMaxOrthogonalityFinderFactory>();
    }
};

class MultiplicativeMaxOrthogonalityFinderFactoryFeature
    : public SharedTypedFeature<SubCollectionFinderFactory> {
public:
    MultiplicativeMaxOrthogonalityFinderFactoryFeature()
        : TypedFeature(
              "multiplicative_max_orthogonality_factory",
              &MultiplicativeMaxOrthogonalityFinderFactoryFeature::func)
    {
    }

    static std::shared_ptr<SubCollectionFinderFactory> func()
    {
        return std::make_shared<MultiplicativeMaxOrthogonalityFinderFactory>();
    }
};
} // namespace

namespace probfd::cli::pdbs {

void add_max_orthogonal_finder_factory_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<AdditiveMaxOrthogonalityFinderFactoryFeature>();
    n.insert_feature_plugin<
        MultiplicativeMaxOrthogonalityFinderFactoryFeature>();
}

} // namespace probfd::cli::pdbs
