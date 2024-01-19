#include "probfd/pdbs/subcollection_finder_factory.h"

#include "downward/plugins/plugin.h"

namespace probfd::pdbs {

static class SubCollectionFinderFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<SubCollectionFinderFactory> {
public:
    SubCollectionFinderFactoryCategoryPlugin()
        : TypedCategoryPlugin("SubCollectionFinderFactory")
    {
    }
} _category_plugin_collection;

} // namespace probfd::pdbs
