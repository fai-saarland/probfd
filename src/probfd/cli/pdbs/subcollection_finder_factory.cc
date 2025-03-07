#include "downward/cli/plugins/plugin.h"

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
} _category_plugin_collection;

} // namespace
