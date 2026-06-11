#include "downward_cli/search_algorithms/search_algorithm_factory_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/search_algorithm.h"
#include "downward/task_dependent_factory_fwd.h"

using namespace language::plugins;

namespace downward::cli::search_algorithms {

void add_search_algorithm_factory_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<TaskDependentFactory<SearchAlgorithm>>(
        "SearchAlgorithmFactory");
    // TODO: add synopsis for the wiki page.
}

} // namespace downward::cli::search_algorithms