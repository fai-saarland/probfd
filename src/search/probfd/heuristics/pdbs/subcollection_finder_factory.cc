#include "probfd/heuristics/pdbs/subcollection_finder_factory.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

static class SubCollectionFinderFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<SubCollectionFinderFactory> {
public:
    SubCollectionFinderFactoryCategoryPlugin()
        : TypedCategoryPlugin("SubCollectionFinderFactory")
    {
    }
} _category_plugin_collection;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd