#include "downward_plugins/plugins/plugin.h"

#include "downward/search_algorithm_factory.h"

using namespace std;

using namespace downward_plugins::plugins;

namespace {

class SearchAlgorithmFactoryCategoryPlugin
    : public TypedCategoryPlugin<SearchAlgorithmFactory> {
public:
    SearchAlgorithmFactoryCategoryPlugin()
        : TypedCategoryPlugin("SearchAlgorithmFactory")
    {
        // TODO: Replace add synopsis for the wiki page.
        // document_synopsis("...");
    }
} _category_plugin;

} // namespace