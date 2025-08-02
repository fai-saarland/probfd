#include "downward/cli/search_algorithms/search_algorithm_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

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
};
}

namespace downward::cli::search_algorithms {

void add_search_algorithm_factory_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<SearchAlgorithmFactoryCategoryPlugin>();
}

} // namespace