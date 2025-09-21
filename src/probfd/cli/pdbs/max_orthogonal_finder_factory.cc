#include "probfd/cli/pdbs/max_orthogonal_finder_factory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/pdbs/max_orthogonal_finder_factory.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {
class AdditiveMaxOrthogonalityFinderFactoryFeature
    : public TypedFeature<SubCollectionFinderFactory> {
public:
    AdditiveMaxOrthogonalityFinderFactoryFeature()
        : TypedFeature("additive_max_orthogonality_factory")
    {
    }

    std::shared_ptr<SubCollectionFinderFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<AdditiveMaxOrthogonalityFinderFactory>();
    }
};

class MultiplicativeMaxOrthogonalityFinderFactoryFeature
    : public TypedFeature<SubCollectionFinderFactory> {
public:
    MultiplicativeMaxOrthogonalityFinderFactoryFeature()
        : TypedFeature("multiplicative_max_orthogonality_factory")
    {
    }

    std::shared_ptr<SubCollectionFinderFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<MultiplicativeMaxOrthogonalityFinderFactory>();
    }
};
} // namespace

namespace probfd::cli::pdbs {

void add_max_orthogonal_finder_factory_feature(RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<AdditiveMaxOrthogonalityFinderFactoryFeature>();
    raw_registry.insert_feature_plugin<
        MultiplicativeMaxOrthogonalityFinderFactoryFeature>();
}

} // namespace probfd::cli::pdbs
