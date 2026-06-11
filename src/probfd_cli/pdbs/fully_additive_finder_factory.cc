#include "probfd_cli/pdbs/fully_additive_finder_factory.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/pdbs/fully_additive_finder_factory.h"

using namespace downward;

using namespace language;

using namespace probfd::pdbs;

using namespace language::plugins;

namespace {
class FullyAdditiveFinderFactoryFeature
    : public TypedFeature<SubCollectionFinderFactory> {
public:
    FullyAdditiveFinderFactoryFeature()
        : TypedFeature("fully_additive_factory")
    {
    }

    std::shared_ptr<SubCollectionFinderFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<FullyAdditiveFinderFactory>();
    }
};
} // namespace

namespace probfd::cli::pdbs {

void add_fully_additive_finder_factory_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<FullyAdditiveFinderFactoryFeature>();
}

} // namespace probfd::cli::pdbs
