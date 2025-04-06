#include "downward/cli/plugins/plugin.h"

#include "downward/search_algorithm.h"

using namespace std;

using namespace downward::cli::plugins;

namespace {

class SearchAlgorithmCategoryPlugin
    : public TypedCategoryPlugin<downward::SearchAlgorithm> {
public:
    SearchAlgorithmCategoryPlugin()
        : TypedCategoryPlugin("SearchAlgorithm")
    {
        // TODO: Replace add synopsis for the wiki page.
        // document_synopsis("...");
    }
} _category_plugin;

} // namespace