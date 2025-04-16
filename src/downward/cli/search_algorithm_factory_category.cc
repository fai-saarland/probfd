#include "downward/cli/plugins/plugin.h"

#include "downward/search_algorithm.h"

#include "downward/task_dependent_factory_fwd.h"

using namespace std;

using namespace downward::cli::plugins;

namespace {

class SearchAlgorithmFactoryCategoryPlugin
    : public TypedCategoryPlugin<
          downward::TaskDependentFactory<downward::SearchAlgorithm>> {
public:
    SearchAlgorithmFactoryCategoryPlugin()
        : TypedCategoryPlugin("SearchAlgorithmFactory")
    {
        // TODO: Replace add synopsis for the wiki page.
        // document_synopsis("...");
    }
} _category_plugin;

} // namespace