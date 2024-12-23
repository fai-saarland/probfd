#include "downward_plugins/plugins/plugin.h"

#include "downward/search_algorithm.h"

using namespace std;

using namespace downward_plugins::plugins;

namespace {

class SearchAlgorithmCategoryPlugin
    : public TypedCategoryPlugin<SearchAlgorithm> {
public:
    SearchAlgorithmCategoryPlugin()
        : TypedCategoryPlugin("SearchAlgorithm")
    {
        // TODO: Replace add synopsis for the wiki page.
        // document_synopsis("...");
    }
} _category_plugin;

} // namespace