#include "probfd/cli/pdbs/subcollection_finder_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/pdbs/subcollection_finder_factory.h"

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {
class SubCollectionFinderFactoryCategoryPlugin
    : public TypedCategoryPlugin<SubCollectionFinderFactory> {
public:
    SubCollectionFinderFactoryCategoryPlugin()
        : TypedCategoryPlugin("SubCollectionFinderFactory")
    {
    }
};
}

namespace probfd::cli::pdbs {

void add_subcollection_finder_factory_category(RawRegistry& raw_registry)
{
    raw_registry
        .insert_category_plugin<SubCollectionFinderFactoryCategoryPlugin>();
}

} // namespace
